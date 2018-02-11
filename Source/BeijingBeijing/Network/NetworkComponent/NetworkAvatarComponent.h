// Copyright 2017-2020 nasuVR All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Network/NetworkComponent/NetworkEntityComponent.h"
#include "NetworkAvatarComponent.generated.h"

/**
 * 
 */
UCLASS()
class BEIJINGBEIJING_API UNetworkAvatarComponent : public UNetworkEntityComponent
{
	GENERATED_BODY()
	
public:
	// Sets default values for this component's properties
	UNetworkAvatarComponent( const FObjectInitializer& ObjectInitializer );

	virtual void BeginPlay() override;
	virtual void EndPlay( const EEndPlayReason::Type endPlayReason ) override;

	// Called every frame
	virtual void TickComponent( float DeltaTime , ELevelTick TickType , FActorComponentTickFunction* ThisTickFunction ) override;
	virtual void linkToKBEObject( EntityBase * networkEntity , bool isLocal ) override;

	UFUNCTION( BlueprintCallable , BlueprintPure , Category = Network )
	bool IsAvatarGuide();

public:

	UFUNCTION( BlueprintCallable , Category = Network )
	void loginToSpace( uint8 mapKey );
	
	UFUNCTION( BlueprintCallable , Category = Network )
	void logoutSpace();

	UFUNCTION( BlueprintCallable , Category = Network )
	void logoutGame();

	void calcPingValue( float deltaTime );
	void sendPingCall( int32 pingIndex );
	void recvPingCall( int32 pingIndex );
	void sendPingValue( int32 ping );

	UFUNCTION( BlueprintCallable , BlueprintPure , Category = Network )
	int32 getPingValue();

	void sendCameraTrans( float deltaTime );
	void sendCameraPos( const FVector &pos );
	void sendCameraRot( const FRotator &rot );
	void OnGetCameraPos( const FVector &pos );
	void OnGetCameraRot( const FRotator &rot );

	virtual void set_position( const UKBEventData *pEventData );
	virtual void updatePosition( const UKBEventData *pEventData );
	virtual void set_direction( const UKBEventData *pEventData );

	void autoVoiceControl( float deltaTime );
	void sendLipData();
	void onGetLipData( const TArray<float> &lipData );
	void getVoiceDataFromVoiceCapture();
	void compressCaptureData();
	void sendVoiceData( const TArray<uint8> &voiceData , int32 realSize );
	void onGetVoiceData( const TArray<uint8> &data );

	void sendCharMoveSpeed();
	void OnGetCharMoveSpeed( float speed );

	UFUNCTION( BlueprintCallable , Category = Network )
	void sendCustomCMD( const FString& cmd , const FString& paramStr , bool isNetworkSend );

	UFUNCTION()
	void onGetCustomCMD( const FString& cmd , const FString& paramStr );

protected:

	UPROPERTY( EditAnywhere , BlueprintReadOnly , Category = Network )
	bool isSyncVoice;

	bool getSyncVoice();

	UFUNCTION( BlueprintCallable , Category = Network )
	void setSyncVoice( bool isMute );

private:

	class AVRCharacterBase * ownerPawn;

	float pingTimer;
	float pingTickInterval;//每隔一段时间 发一个Ping
	float pingSendTime;//发送时时间
	int32 curPingIndex;//第几次发送
	bool isRecvWithinInternal;//是否在间隔内收到回响

	TArray<float> lipMorphData;
	TArray<float> lastLipMorphData;
	TArray<float> emptyMorphData;
	float lipMorphTimer;

	TSharedPtr<class IVoiceCapture> voiceCapture;
	TSharedPtr<class IVoiceEncoder> voiceEncoder;

	TArray<uint8> rawCaptureData;
	TArray<uint8> compressedData;
	int32 curRawCaptureData;
	int32 maxRawCaptureDataSize;
	int32 maxCompressedDataSize;

	bool isPauseSyncVoice;
	bool lastSyncVoice;

	float lastCharMoveSpeed;
};
