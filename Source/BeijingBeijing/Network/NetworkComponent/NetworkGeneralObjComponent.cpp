// Copyright 2017-2020 nasuVR All Rights Reserved.

#include "NetworkGeneralObjComponent.h"
#include "Network/KBE_Entity/GeneralObj.h"
#include "kismet/GameplayStatics.h"
#include "Network/NetworkGameMode.h"
#include "PawnAndHand/VRCharacterBase.h"
#include "Pickup/PickupCommonActor.h"


UNetworkGeneralObjComponent::UNetworkGeneralObjComponent( const FObjectInitializer& ObjectInitializer )
	:Super( ObjectInitializer )
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UNetworkGeneralObjComponent::sendPickupEvent( int32 avatarID , bool isLeft )
{
	if ( networkEntity == nullptr ) return;
	if ( !isLocallyControl )return;
	if ( !networkEntity->inWorld() )return;

	GeneralObj *obj = static_cast<GeneralObj*>( networkEntity );
	obj->pickupObject( avatarID , isLeft );
}

void UNetworkGeneralObjComponent::onGetPickupEvent( int32 avatarID , bool isLeft )
{
	if ( networkEntity == nullptr ) return;
	if ( isLocallyControl )return;
	if ( !networkEntity->inWorld() )return;
	
	APickupCommonActor *pickActor = Cast<APickupCommonActor>( GetOwner() );
	if ( !pickActor ) return;

	ANetworkGameMode *gameMode = Cast<ANetworkGameMode>( UGameplayStatics::GetGameMode( this ) );
	UNetworkBaseComponent *networkComp = gameMode->findGameEntity( avatarID );
	if ( !networkComp ) return;

	AVRCharacterBase *pawn = Cast<AVRCharacterBase>( networkComp->GetOwner() );
	IHandInterface::Execute_OnPickup( pickActor , isLeft ? pawn->leftController : pawn->rightController );
}

void UNetworkGeneralObjComponent::sendDropEvent( int32 avatarID , bool isLeft )
{
	if ( networkEntity == nullptr ) return;
	if ( !isLocallyControl )return;
	if ( !networkEntity->inWorld() )return;

	GeneralObj *obj = static_cast<GeneralObj*>( networkEntity );
	obj->dropObject( avatarID , isLeft );
}

void UNetworkGeneralObjComponent::onGetDropEvent( int32 avatarID , bool isLeft )
{
	if ( networkEntity == nullptr ) return;
	if ( isLocallyControl )return;
	if ( !networkEntity->inWorld() )return;

	APickupCommonActor *pickActor = Cast<APickupCommonActor>( GetOwner() );
	if ( !pickActor ) return;

	ANetworkGameMode *gameMode = Cast<ANetworkGameMode>( UGameplayStatics::GetGameMode( this ) );
	UNetworkBaseComponent *networkComp = gameMode->findGameEntity( avatarID );
	if ( !networkComp ) return;

	AVRCharacterBase *pawn = Cast<AVRCharacterBase>( networkComp->GetOwner() );
	IHandInterface::Execute_OnDrop( pickActor , isLeft ? pawn->leftController : pawn->rightController );
}
