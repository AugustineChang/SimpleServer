#include "TimeLimiter.h"
#include "HAL/PlatformFilemanager.h"
#include <fstream>
#include <iostream>
using namespace std;

UTimeLimiter* UTimeLimiter::instance = nullptr;

UTimeLimiter::UTimeLimiter() : filePath( TEXT( "StreamAssets/" ) ) , fileName( TEXT("nasuvr.nasu") )
{
	filePath = FPaths::Combine( *FPaths::GameContentDir() , *filePath );
	fullPath = filePath + fileName;

	password = new int32[4]{ 8,3,2,9 };

	IPlatformFile &file = FPlatformFileManager::Get().GetPlatformFile();
	hasEncryptFile = file.FileExists( *fullPath );
	if ( hasEncryptFile )
	{
		if ( !loadDataFromFile() )
		{
			hasEncryptFile = false;
			isForceNetwork = true;
			authorityName = TEXT( "no user" );
		}
	}
	else
	{
		isForceNetwork = true;
		authorityName = TEXT( "no user" );
	}
}

UTimeLimiter* UTimeLimiter::getInstance()
{
	if ( instance == nullptr )
	{
		instance = new UTimeLimiter();
	}
	return instance;
}

void UTimeLimiter::checkNetworkAndFile( bool network , const FString & serverData )
{
	hasNetwork = network;

	const FDateTime &today = FDateTime::Today();
	if ( hasNetwork )
	{
		analysisRawData( serverData );

		if ( createDataDirectory() )
		{
			saveDataToFile();
		}
	}
	else if ( !isForceNetwork )
	{
		if ( !hasEncryptFile )
		{
			deadline = FDateTime( today.GetTicks() - ETimespan::TicksPerDay );
			lastDate = today;
		}
	}
	else
	{
		deadline = FDateTime( today.GetTicks() - ETimespan::TicksPerDay );
		lastDate = today;
	}
}

bool UTimeLimiter::createDataDirectory()
{
	IPlatformFile &file = FPlatformFileManager::Get().GetPlatformFile();
	if ( !file.DirectoryExists( *filePath ) )
	{
		file.CreateDirectoryTree( *filePath );

		if ( !file.DirectoryExists( *filePath ) )
		{
			return false;
		}
	}

	return true;
}

bool UTimeLimiter::loadDataFromFile()
{
	IPlatformFile &file = FPlatformFileManager::Get().GetPlatformFile();
	IFileHandle *fileHandle = file.OpenRead( *fullPath );
	if ( fileHandle )
	{
		int64 fileSize = fileHandle->Size();
		uint8 *pointer_8 = reinterpret_cast<uint8*>( FMemory::Malloc( fileSize ) );

		fileHandle->Read( pointer_8 , fileSize );
		decryptData( pointer_8 , fileSize );
		delete fileHandle;

		int32 curPos = 0;
		int64* temp = reinterpret_cast<int64*>( pointer_8 + curPos );
		lastDate = FDateTime( *temp );//lastData
		curPos += sizeof( int64 );
		if ( curPos >= fileSize ) return false;

		temp = reinterpret_cast<int64*>( pointer_8 + curPos );
		deadline = FDateTime( *temp );//deadline
		curPos += sizeof( int64 );
		if ( curPos >= fileSize ) return false;

		int32* strLen = reinterpret_cast<int32*>( pointer_8 + curPos );//string length
		curPos += sizeof( int32 );
		if ( curPos >= fileSize ) return false;

		ANSICHAR* strBytes = reinterpret_cast<ANSICHAR*>( pointer_8 + curPos );
		FUTF8ToTCHAR echoStrFString( strBytes , *strLen );
		authorityName = FString( *strLen , echoStrFString.Get() );
		curPos += *strLen;
		if ( curPos >= fileSize ) return false;

		isForceNetwork = *( pointer_8 + curPos ) != (uint8) 0;

 		FMemory::Free( pointer_8 );
		return true;
	}
	return false;
}

void UTimeLimiter::saveDataToFile()
{
	IPlatformFile &file = FPlatformFileManager::Get().GetPlatformFile();
	IFileHandle *fileHandle = file.OpenWrite( *fullPath );
	if ( fileHandle )
	{
		FTCHARToUTF8 echoStrUTF8( *authorityName );
		int32 nameLen = echoStrUTF8.Length();

		int32 byteLen = sizeof( int64 ) * 2 + sizeof( int32 ) + nameLen + 1;
		int32 bytePerInt = ( sizeof( int32 ) / sizeof( uint8 ) );
		if ( byteLen % bytePerInt > 0 )
		{
			byteLen += ( bytePerInt - byteLen % bytePerInt );
		}

		uint8 *pointer_8 = reinterpret_cast<uint8*>( FMemory::Malloc( byteLen ) );
		

		int32 curPos = 0;
		int64* temp = reinterpret_cast<int64*>( pointer_8 + curPos );
		*temp = lastDate.GetTicks();//lastData
		curPos += sizeof( int64 );

		temp = reinterpret_cast<int64*>( pointer_8 + curPos );
		*temp = deadline.GetTicks();//deadline
		curPos += sizeof( int64 );
		
		int32* temp2 = reinterpret_cast<int32*>( pointer_8 + curPos );
		*temp2 = nameLen;//string length
		curPos += sizeof( int32 );
		
		const uint8* strBytes = reinterpret_cast<const uint8*>( echoStrUTF8.Get() );
		FMemory::Memcpy( pointer_8 + curPos , strBytes , nameLen );//authorityName
		curPos += nameLen;

		*( pointer_8 + curPos ) = isForceNetwork ? (uint8) 1 : (uint8) 0;//forceNetwork
		curPos += 1;

		for ( int32 i = curPos; i < byteLen; ++i )
		{
			*( pointer_8 + i ) = (uint8) 0;
		}

		encryptData( pointer_8 , byteLen );
		fileHandle->Write( pointer_8 , byteLen );
		delete fileHandle;

		FMemory::Free( pointer_8 );
	}
}

void UTimeLimiter::analysisRawData( const FString &rawData )
{
	if ( rawData == TEXT( "no user" ) )
	{
		const FDateTime &today = FDateTime::Today();
		deadline = FDateTime( today.GetTicks() - ETimespan::TicksPerDay );
		lastDate = today;
		isTimeValid_Server = false;
		return;
	}

	FString serverTimeStr;
	FString deadlineStr;
	FString isValidStr;
	FString temp;

	rawData.Split( TEXT( "," ) , &serverTimeStr , &temp );
	temp.Split( TEXT( "," ) , &deadlineStr , &isValidStr );

	analysisDatetime( serverTimeStr , lastDate );
	analysisDatetime( deadlineStr , deadline );
	isTimeValid_Server = FCString::ToBool( *isValidStr );
}

void UTimeLimiter::analysisDatetime( const FString &dateStr , FDateTime &datetime )
{
	FString year;
	FString month;
	FString day;
	FString temp;

	dateStr.Split( TEXT( "-" ) , &year , &temp );
	temp.Split( TEXT( "-" ) , &month , &day );

	datetime = FDateTime(
		FCString::Atoi( *year ) ,
		FCString::Atoi( *month ) ,
		FCString::Atoi( *day ) );
}

void UTimeLimiter::encryptData( uint8* data , int32 byteLen )
{
	int32* tempPointer = reinterpret_cast<int32*>( data );
	int32 bytePerInt = ( sizeof( int32 ) / sizeof( uint8 ) );

	int32 realLen = byteLen / bytePerInt;

	xxtea( tempPointer , realLen , password );
}

void UTimeLimiter::decryptData( uint8* data , int32 byteLen )
{
	int32* tempPointer = reinterpret_cast<int32*>( data );
	int32 bytePerInt = ( sizeof( int32 ) / sizeof( uint8 ) );

	int32 realLen = byteLen / bytePerInt;

	xxtea( tempPointer , -realLen , password );
}

#define MX ( z >> 5 ^ y << 2 ) + ( y >> 3 ^ z << 4 ) ^ ( sum^y ) + ( k[p & 3 ^ e] ^ z );

int32 UTimeLimiter::xxtea( int32* v , int32 n , int32 *k )
{
	uint32 z = v[n - 1];
	uint32 y = v[0];
	uint32 sum = 0;
	uint32 e = 0;
	uint32 delta = 0x9e3779b9;
	int32 p , q;

	if ( n > 1 )//加密部分
	{
		q = 6 + 52 / n;
		while ( q-- > 0 )
		{
			sum += delta;
			e = ( sum >> 2 ) & 3;
			for ( p = 0; p < n - 1; p++ )
			{
				y = v[p + 1] , z = v[p] += MX;
			}

			y = v[0];
			z = v[n - 1] += MX;
		}
		return 0;
	}
	else if ( n < -1 )//解密部分
	{
		n = -n;
		q = 6 + 52 / n;
		sum = q*delta;
		while ( sum != 0 )
		{
			e = ( sum >> 2 ) & 3;
			for ( p = n - 1; p > 0; p-- )
			{
				z = v[p - 1] , y = v[p] -= MX;
			}
			
			z = v[n - 1];
			y = v[0] -= MX;
			sum -= delta;
		}
		return 0;
	}
	return 1;
}

bool UTimeLimiter::isTimeValid()
{
	if ( hasNetwork )
	{
		return isTimeValid_Server;
	}
	else
	{
		const FDateTime &today = FDateTime::Today();
		return ( today >= lastDate ) && ( today <= deadline );
	}
}

bool UTimeLimiter::getIsForceNetwork()
{
	return isForceNetwork;
}

const FString& UTimeLimiter::getAuthorityName()
{
	return authorityName;
}

FString UTimeLimiter::getEncryptString( const FString &rawStr )
{
	FTCHARToUTF8 echoStrUTF8( *rawStr );
	
	int32 strLen = echoStrUTF8.Length();
	int32 byteLen = strLen;
	int32 bytePerInt = ( sizeof( int32 ) / sizeof( uint8 ) );
	if ( byteLen % bytePerInt > 0 )
	{
		byteLen += ( bytePerInt - byteLen % bytePerInt );
	}
	uint8 *pointer_8 = reinterpret_cast<uint8*>( FMemory::Malloc( byteLen ) );

	const uint8* strBytes = reinterpret_cast<const uint8*>( echoStrUTF8.Get() );
	FMemory::Memcpy( pointer_8 , strBytes , byteLen );

	for ( int32 i = strLen; i < byteLen; ++i )
	{
		*( pointer_8 + i ) = (uint8) 32;//补上空格
	}

	encryptData( pointer_8 , byteLen );

	const char* strBytes_encry = reinterpret_cast<const char*>( pointer_8 );
	FMemory::Free( pointer_8 );

	return FString( byteLen , ANSI_TO_TCHAR( strBytes_encry ) );
}

FString UTimeLimiter::getDecryptString( TArray<uint8> &encryData , int32 dataLen )
{
	decryptData( encryData.GetData() , dataLen );

	return FString( ANSI_TO_TCHAR( reinterpret_cast<const char*>( encryData.GetData() ) ) );
}

FString UTimeLimiter::getDecryptString( const FString &encryptStr )
{
	ANSICHAR* encryANSI = TCHAR_TO_ANSI( *encryptStr );
	int32 byteLen = encryptStr.Len();

	uint8 *pointer_8 = reinterpret_cast<uint8*>( FMemory::Malloc( byteLen ) );

	const uint8* strBytes = reinterpret_cast<const uint8*>( encryANSI );
	FMemory::Memcpy( pointer_8 , strBytes , byteLen );

	decryptData( pointer_8 , byteLen );

	ANSICHAR* strBytes_encry = reinterpret_cast<ANSICHAR*>( pointer_8 );
	FUTF8ToTCHAR echoStrFString( strBytes_encry , byteLen );
	FMemory::Free( pointer_8 );

	return FString( byteLen , echoStrFString.Get() );
}
