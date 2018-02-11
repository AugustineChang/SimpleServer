// Copyright 2017-2020 nasuVR All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Network/NetworkComponent/NetworkEntityComponent.h"
#include "NetworkHandComponent.generated.h"

UCLASS()
class BEIJINGBEIJING_API UNetworkHandComponent : public UNetworkEntityComponent
{
	GENERATED_BODY()
	
public:

	UNetworkHandComponent( const FObjectInitializer& ObjectInitializer );

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent( float DeltaTime , ELevelTick TickType , FActorComponentTickFunction* ThisTickFunction ) override;

public:

	void sendHandTrans( float deltaTime );
	void sendHandPos( const FVector &pos );
	void sendHandRot( const FRotator &rot );
	virtual void set_position( const UKBEventData *pEventData );
	virtual void updatePosition( const UKBEventData *pEventData );
	virtual void set_direction( const UKBEventData *pEventData );
	
	void sendHandInputAction( uint8 key , uint8 val );
	void onGetInputAction( uint8 key , uint8 val );

	void sendHandInputAxis( uint8 key , float val );
	void onGetInputAxis( uint8 key , float val );

	void sendLaserState( uint8 laser );
	void onGetLaserState( uint8 laser );

	void sendGrabState( uint8 grab );
	void onGetGrabState( uint8 grab );

protected:

	class AMotionControllerBase * ownerHand;
};
