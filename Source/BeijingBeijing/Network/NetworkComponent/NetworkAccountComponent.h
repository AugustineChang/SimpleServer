// Copyright 2017-2020 nasuVR All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Network/NetworkComponent/NetworkBaseComponent.h"
#include "NetworkAccountComponent.generated.h"

class UKBEventData;
class ANetworkGameMode;

/**
 * 
 */
UCLASS()
class BEIJINGBEIJING_API UNetworkAccountComponent : public UNetworkBaseComponent
{
	GENERATED_BODY()
	
public:
	// Sets default values for this component's properties
	UNetworkAccountComponent( const FObjectInitializer& ObjectInitializer );

	void onGameStarted( bool isSuccess );

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay( const EEndPlayReason::Type endPlayReason ) override;

	UFUNCTION( BlueprintCallable , Category = Network )
	void autoLogin();

protected:
	
	UFUNCTION( BlueprintCallable , Category = Network )
	void reqStartGame( const FString &name , uint8 authority , uint8 charType );

	UFUNCTION( BlueprintCallable , Category = Network )
	void reqLogout();

	//////////////////////////////////////////////////////////////////////////////
	
	void onDisconnected( const UKBEventData *pEventData );
	void onConnectionState( const UKBEventData *pEventData );

	//////////////////////////////////////////////////////////////////////////////
	void onCreateAccountResult( const UKBEventData *pEventData );
	void onLoginFailed( const UKBEventData *pEventData );
	void onVersionNotMatch( const UKBEventData *pEventData );
	void onScriptVersionNotMatch( const UKBEventData *pEventData );
	void onLoginBaseappFailed( const UKBEventData *pEventData );
	void onLoginSuccessfully( const UKBEventData *pEventData );
	void onLoginBaseapp( const UKBEventData *pEventData );
	void Loginapp_importClientMessages( const UKBEventData *pEventData );
	void Baseapp_importClientMessages( const UKBEventData *pEventData );
	void Baseapp_importClientEntityDef( const UKBEventData *pEventData );

private:

	ANetworkGameMode * gameMode;

	bool isStartGame;
	FString userName;
	FString password;
	int32 totalNum;
	int32 curNum;
};
