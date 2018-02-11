// Fill out your copyright notice in the Description page of Project Settings.

#include "VRGameInstance.h"
#include "ReceiveThread.h"
#include "Other/TimeLimiter.h"
#include "Other/BP_Util.h"
#include "ConnectSocketThread.h"
#include "Other/DataTables.h"
#include "UObject/ConstructorHelpers.h"
#include "Serialization/Csv/CsvParser.h"
#include "BeijingBeijing/WeatherRequest.h"


UVRGameInstance::UVRGameInstance( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer ) , isGetTimeData( false ) , curSpotListName()
{
	authorityName = UTimeLimiter::getInstance()->getAuthorityName();
	isForceNetwork = UTimeLimiter::getInstance()->getIsForceNetwork();

	GetTimerManager().SetTimer( timerHandle , this , &UVRGameInstance::timerCallBack , 0.5f , true );
	
	loadCSVTable( TEXT( "BigScreenTable" ) );
	loadCSVTable( TEXT( "ScenicSpotTable" ) );

	initScenicSpotList();
	UWeatherRequest::getSingleton();
}

bool UVRGameInstance::getTableData( const FString& tableName , const FString& rowKey , const FString& columnKey , FString& outStr )
{
	UTable **table = tableMap.Find( tableName );
	if ( !table ) return false;

	UTableRow **row = ( *table )->tableMap.Find( rowKey );
	if ( !row ) return false;

	FString *result = ( *row )->rowMap.Find( columnKey );
	if ( !result ) return false;

	outStr = *result;
	return true;
}

bool UVRGameInstance::getTableData( const FString& tableName , const FString& rowKey , UTableRow *&outRow )
{
	outRow = nullptr;
	
	UTable **table = tableMap.Find( tableName );
	if ( !table ) return false;

	UTableRow **row = ( *table )->tableMap.Find( rowKey );
	if ( !row ) return false;

	outRow = *row;
	return true;
}

bool UVRGameInstance::getTableData( const FString& tableName , UTable *&outTable )
{
	outTable = nullptr;

	UTable **table = tableMap.Find( tableName );
	if ( !table ) return false;

	outTable = *table;
	return true;
}

void UVRGameInstance::loadCSVTable( const FString &tableName )
{
	FString rootPath = FPaths::Combine( *FPaths::GameContentDir() , TEXT( "StreamAssets/DataTables/" ) );
	FString filePath = rootPath + tableName + TEXT( ".csv" );
	if ( !FPaths::FileExists( filePath ) ) return;

	FString FileContent;
	//Read the csv file
	FFileHelper::LoadFileToString( FileContent , *filePath );

	FCsvParser csvParser( FileContent );
	TArray<TArray<const TCHAR*>> csvRows = csvParser.GetRows();

	int32 rowNum = csvRows.Num();
	int32 columnNum = csvRows[0].Num();

	if ( rowNum <= 1 || columnNum <= 1 ) return;

	UTable *curTable = NewObject<UTable>();
	for ( int32 y = 1; y < rowNum; ++y )
	{
		for ( int32 x = 1; x < columnNum; ++x )
		{
			FString rowKey = csvRows[y][0];
			FString columnKey = csvRows[0][x];

			UTableRow *curRow = nullptr;
			if ( curTable->tableMap.Num() < y )
			{
				curRow = NewObject<UTableRow>();
				curRow->rowKey = rowKey;
				curTable->tableMap.Add( rowKey , curRow );
			}
			else
			{
				curRow = curTable->tableMap[rowKey];
			}
			curRow->rowMap.Add( columnKey , csvRows[y][x] );
		}
	}

	tableMap.Add( tableName , curTable );
}

bool UVRGameInstance::CreateSocket(FString IPStr, int32 port)
{
	FIPv4Address::Parse(IPStr, ip);

	TSharedPtr<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	addr->SetIp(ip.Value);
	addr->SetPort(port);

	if (nullptr == Host) {
		Host = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("DEFAULT"), false);
	}
	
	m_ConnThread = FRunnableThread::Create(new FConnectSocketThread(Host, addr), TEXT("ConnThread"));

	return (Host->GetConnectionState() == ESocketConnectionState::SCS_Connected ? true : false);
}

bool UVRGameInstance::SocketSend(FString message)
{
	if (message == "") {
		message = "no user";
	}
	TCHAR *seriallizedChar = message.GetCharArray().GetData();
	int32 size = FCString::Strlen(seriallizedChar) + 1;
	int32 sent = 0;

	if (Host->Send((uint8*)TCHAR_TO_UTF8(seriallizedChar), size, sent)) {
		UE_LOG(LogTemp, Warning, TEXT("__send succeed!"));
		return true;
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("__send failed!"));
		return false;
	}
}

bool UVRGameInstance::SocketReceive()
{
	m_RecvThread = FRunnableThread::Create(new FReceiveThread(Host, this), TEXT("RecvThread"));
	return true;
}

FString UVRGameInstance::StringFromBinaryArray(TArray<uint8> BinaryArray)
{
	return FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(BinaryArray.GetData())));
}

FString UVRGameInstance::GetEncryptString( const FString& strData , bool isEncrypt )
{
	if ( isEncrypt )
	{
		return UTimeLimiter::getInstance()->getEncryptString( strData );
	}
	else
	{
		return UTimeLimiter::getInstance()->getDecryptString( strData );
	}
}

void UVRGameInstance::OnTimeLimitCallback( bool hasNetwork , const FString &serverData )
{
	UTimeLimiter::getInstance()->checkNetworkAndFile( hasNetwork , serverData );

	isGetTimeData = true;
	isTimeValid = UTimeLimiter::getInstance()->isTimeValid();
}

void UVRGameInstance::timerCallBack()
{
	if ( isGetTimeData )
	{
		GetTimerManager().ClearTimer( timerHandle );
		OnGetTimeValid.Broadcast( isTimeValid );
	}
}

void UVRGameInstance::initScenicSpotList()
{
	UTable *screenTable = nullptr;
	if ( !getTableData( TEXT( "BigScreenTable" ) , screenTable ) ) return;

	TArray<FString> allRows = screenTable->getRowNames();
	for ( const FString &rowName : allRows )
	{
		UTableRow *curRow = nullptr;
		if ( !screenTable->getTableData( rowName , curRow ) ) continue;

		FString spotListStr;
		if ( !curRow->getRowData( TEXT( "mapList" ) , spotListStr ) ) continue;

		UScenicSpotList *spotList = NewObject<UScenicSpotList>();

		bool isGoOn = true;
		FString tempLeft;
		FString tempRight;
		while ( isGoOn )
		{
			isGoOn = spotListStr.Split( TEXT( "-" ) , &tempLeft , &tempRight );

			if ( !isGoOn )
			{
				spotList->spotList.Add( spotListStr );
			}
			else
			{
				spotListStr = tempRight;
				spotList->spotList.Add( tempLeft );
			}
		}

		scenicSpotMap.Add( rowName , spotList );
	}
}

FString UVRGameInstance::getNextLevel( const FString &curLevel )
{
	if ( curSpotListName.IsEmpty() ) return TEXT( "None" );

	UScenicSpotList **spotList = scenicSpotMap.Find( curSpotListName );
	if ( spotList == nullptr ) return TEXT( "None" );
	
	int32 outIndex;
	if ( ( *spotList )->spotList.Find( curLevel , outIndex ) )
	{
		int32 len = ( *spotList )->spotList.Num();
		if ( outIndex >= len - 1 ) return TEXT( "None" );
		else return ( *spotList )->spotList[outIndex + 1];
	}
	else return TEXT( "None" );
}

FString UVRGameInstance::getFisrtLevel()
{
	if ( curSpotListName.IsEmpty() ) return TEXT( "None" );

	UScenicSpotList **spotList = scenicSpotMap.Find( curSpotListName );
	if ( spotList == nullptr ) return TEXT( "None" );

	return ( *spotList )->spotList[0];
}

void UVRGameInstance::setSpotListName( const FString &spotListName )
{
	curSpotListName = spotListName;
}

void UVRGameInstance::clearSpotListName()
{
	curSpotListName.Empty();
}
