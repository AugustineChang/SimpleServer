// Copyright@2017

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "KBECommon.h"
#include "NetworkGameMode.generated.h"

class UNetworkBaseComponent;
class UKBEMain;
class UKBEventData;

UCLASS( Config = Game )
class BEIJINGBEIJING_API ANetworkGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ANetworkGameMode();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay( const EEndPlayReason::Type EndPlayReason ) override;

public:

	UFUNCTION( BlueprintCallable , Category = "KBEngine" )
	UNetworkBaseComponent* findGameEntity( int entityID );

	UFUNCTION( BlueprintCallable , Category = "KBEngine" )
	void addGameEntity( int entityID , UNetworkBaseComponent* entity );

	UFUNCTION( BlueprintCallable , Category = "KBEngine" )
	void removeGameEntity( int entityID );

	inline UKBEMain *getKBEMain()
	{
		return mainComponent;
	}

	void checkOpenLevel();

	UFUNCTION( BlueprintCallable , Category = "KBEngine" )
	const TArray<AActor*> &getAllPlayers();

	///////////////////////////////////////////////////
	void onKicked( const UKBEventData *pEventData );
	void onDisconnected( const UKBEventData *pEventData );

	///////////////////////////////////////////////////
	void onEnterWorld( const UKBEventData* pEventData );
	void onLeaveWorld( const UKBEventData* pEventData );
	void onEnterSpace( const UKBEventData* pEventData );
	void onLeaveSpace( const UKBEventData* pEventData );

protected:
	
	UNetworkBaseComponent *createActorByName( const FString& className , const FTransform &SpawnTransform , int32 entityID , bool isLocal );
	void destroyActorByID( int32 entityID , bool isPlayer );
	
	void createAvatar( int32 entityID , bool isLocal , const FTransform& spawnTrans );
	void createHand( int32 entityID );
	void checkUnhandledHands( int avatarID );

protected:

	UPROPERTY( VisibleAnywhere , Category = Network )
	USceneComponent * gameModeRoot;

	UPROPERTY( VisibleAnywhere , BlueprintReadOnly , Category = Network )
	UKBEMain *mainComponent;

	UPROPERTY( EditAnywhere , Category = Network )
	bool isLoadFromConfig;

	UPROPERTY( Config , VisibleAnywhere , Category = Network )
	FString ipAddress;

	UPROPERTY( Config , VisibleAnywhere , Category = Network )
	int32 port;

	UPROPERTY( Config , VisibleAnywhere , Category = Network )
	int32 sendBufferSize;

	UPROPERTY( Config , VisibleAnywhere , Category = Network )
	int32 recvBufferSize;

	UPROPERTY( EditDefaultsOnly , Category = KBEngine )
	TMap<FString , UClass*> name2Class;

	UPROPERTY( BlueprintReadOnly , Category = KBEngine )
	TMap<int32 , UNetworkBaseComponent*> networkEntityMap;

	UPROPERTY( EditDefaultsOnly , BlueprintReadOnly , Category = KBEngine )
	int32 mapKey;

protected:

	TArray<AActor*> allPlayers;
	TMap<ENTITY_ID , TArray<ENTITY_ID>> unhandledHands;
};
