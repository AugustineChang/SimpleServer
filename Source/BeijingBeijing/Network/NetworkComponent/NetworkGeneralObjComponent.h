// Copyright 2017-2020 nasuVR All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Network/NetworkComponent/NetworkEntityComponent.h"
#include "NetworkGeneralObjComponent.generated.h"

/**
 * 
 */
UCLASS()
class BEIJINGBEIJING_API UNetworkGeneralObjComponent : public UNetworkEntityComponent
{
	GENERATED_BODY()
	
public:

	UNetworkGeneralObjComponent( const FObjectInitializer& ObjectInitializer );
	
public:

	void sendPickupEvent( int32 avatarID , bool isLeft );
	void onGetPickupEvent( int32 avatarID , bool isLeft );

	void sendDropEvent( int32 avatarID , bool isLeft );
	void onGetDropEvent( int32 avatarID , bool isLeft );
};
