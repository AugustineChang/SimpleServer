// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "Networking.h"
#include "VRGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FOnTimeLimit , bool , isValid );

class UDataTable;
class UTable;
class UScenicSpotList;

UCLASS( Config = Game )
class BEIJINGBEIJING_API UVRGameInstance : public UGameInstance
{
	GENERATED_BODY()

	UVRGameInstance( const FObjectInitializer& ObjectInitializer );

public:
	
	bool getTableData( const FString& tableName , const FString& rowKey , const FString& columnKey , FString &outStr );
	bool getTableData( const FString& tableName , const FString& rowKey , UTableRow *&outRow );
	bool getTableData( const FString& tableName , UTable *&outTable );
	void loadCSVTable( const FString &tableName );

	//////////////////////////////////////////////////////////////////
	UFUNCTION( BlueprintCallable , Category = "TimeLimit" )
	void OnTimeLimitCallback( bool hasNetwork , const FString &serverData );

	UPROPERTY( BlueprintAssignable , Category = "TimeLimit" )
	FOnTimeLimit OnGetTimeValid;

	void timerCallBack();

	//////////////////////////////////////////////////////////////////

	void initScenicSpotList();
	UFUNCTION( BlueprintCallable , BlueprintPure , Category = "NextMap" )
	FString getNextLevel( const FString &curLevel );

	UFUNCTION( BlueprintCallable , BlueprintPure , Category = "NextMap" )
	FString getFisrtLevel();

	UFUNCTION( BlueprintCallable , Category = "NextMap" )
	void setSpotListName( const FString &spotListName );

	UFUNCTION( BlueprintCallable , Category = "NextMap" )
	void clearSpotListName();

protected:

	UPROPERTY( VisibleAnywhere , BlueprintReadOnly , Category = "TimeLimit" )
	FString authorityName;

	UPROPERTY( VisibleAnywhere , BlueprintReadOnly , Category = "TimeLimit" )
	bool isForceNetwork;

	UPROPERTY( BlueprintReadOnly , Category = "TimeLimit" )
	bool isGetTimeData;

	UPROPERTY( BlueprintReadOnly , Category = "TimeLimit" )
	bool isTimeValid;

	FTimerHandle timerHandle;

	UPROPERTY()
	TMap<FString,UTable*> tableMap;

	UPROPERTY()
	TMap<FString, UScenicSpotList*> scenicSpotMap;
	FString curSpotListName;

//ue socket
public:

	UFUNCTION(BlueprintCallable, Category = "Socket")
	bool CreateSocket(FString IPStr, int32 port);

	UFUNCTION(BlueprintCallable, Category = "Socket")
	bool SocketSend(FString message);

	UFUNCTION(BlueprintCallable, Category = "Socket")
	bool SocketReceive();

	FString StringFromBinaryArray(TArray<uint8> BinaryArray);

	UFUNCTION( BlueprintCallable , BlueprintPure , Category = "Socket" )
	FString GetEncryptString( const FString& strData , bool isEncrypt );

public:
	FSocket *Host;
	FIPv4Address ip;
	FRunnableThread *m_RecvThread;
	FRunnableThread *m_ConnThread;

	UPROPERTY(BlueprintReadWrite, Category = "Socket")
	bool bHasAuthority;

	UPROPERTY(BlueprintReadWrite, Category = "Socket")
	bool bConToSockServer;

	UPROPERTY(BlueprintReadWrite, Category = "Socket")
	int endTime;

	UPROPERTY( BlueprintReadWrite , Category = "Network" )
	bool isOpenLevel;
};
