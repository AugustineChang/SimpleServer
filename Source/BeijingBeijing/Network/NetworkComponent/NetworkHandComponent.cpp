// Copyright 2017-2020 nasuVR All Rights Reserved.

#include "NetworkHandComponent.h"
#include "Network/KBE_Entity/Hand.h"
#include "PawnAndHand/MotionControllerBase.h"

UNetworkHandComponent::UNetworkHandComponent( const FObjectInitializer& ObjectInitializer )
	:Super( ObjectInitializer )
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UNetworkHandComponent::BeginPlay()
{
	Super::BeginPlay();

	ownerHand = Cast<AMotionControllerBase>( GetOwner() );
	if ( ownerHand )
	{
		setupEntity( ownerHand->GetRootComponent() , true );

		KBENGINE_REGISTER_EVENT( "set_position" , set_position );
		KBENGINE_REGISTER_EVENT( "updatePosition" , updatePosition );
		KBENGINE_REGISTER_EVENT( "set_direction" , set_direction );
	}
}

void UNetworkHandComponent::TickComponent( float DeltaTime , ELevelTick TickType , FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime , TickType , ThisTickFunction );

	if ( !getHasNetwork() || !isLocallyControl ) return;

	sendHandTrans( DeltaTime );
}

///////////////////////////////////Move///////////////////////////////////////////
void UNetworkHandComponent::sendHandTrans( float deltaTime )
{
	FVector newLoc;
	FRotator newRot;
	bool isPosValid;
	bool isRotValid;

	checkSendTrans( 0 , newLoc , newRot , isPosValid , isRotValid );

	if ( isPosValid ) sendHandPos( newLoc );
	if ( isRotValid ) sendHandRot( newRot );
}

void UNetworkHandComponent::sendHandPos( const FVector &pos )
{
	if ( networkEntity == nullptr ) return;

	Hand *hand = static_cast<Hand*>( networkEntity );
	hand->sendHandLocation( pos );
}

void UNetworkHandComponent::sendHandRot( const FRotator &rot )
{
	if ( networkEntity == nullptr ) return;

	Hand *hand = static_cast<Hand*>( networkEntity );
	hand->sendHandRotation( rot );
}

void UNetworkHandComponent::set_position( const UKBEventData *pEventData )
{
	if ( networkEntity == nullptr )return;
	if ( isLocallyControl )return;

	const UKBEventData_set_position *pData = Cast<UKBEventData_set_position>( pEventData );
	if ( pData->entityID != networkEntity->id() ) return;
	
	if ( ownerHand )
	{
		if ( getIsSyncRelative( 0 ) )
			ownerHand->SetActorRelativeLocation( pData->position );
		else
			ownerHand->SetActorLocation( pData->position );
	}
}

void UNetworkHandComponent::updatePosition( const UKBEventData *pEventData )
{
	if ( networkEntity == nullptr )return;
	if ( isLocallyControl )return;

	const UKBEventData_updatePosition *pData = Cast<UKBEventData_updatePosition>( pEventData );
	if ( pData->entityID != networkEntity->id() ) return;

	onGetPosition( 0 , pData->position );
}

void UNetworkHandComponent::set_direction( const UKBEventData *pEventData )
{
	if ( networkEntity == nullptr )return;
	if ( isLocallyControl )return;

	const UKBEventData_set_direction *pData = Cast<UKBEventData_set_direction>( pEventData );
	if ( pData->entityID != networkEntity->id() ) return;
	
	FRotator ueRot;
	KBDir2UE4Dir( ueRot , pData->direction );
	onGetRotation( 0 , ueRot );
}

//////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////Input//////////////////////////////////////////
void UNetworkHandComponent::sendHandInputAction( uint8 key , uint8 val )
{
	if ( networkEntity == nullptr ) return;
	if ( !isLocallyControl )return;
	if ( !networkEntity->inWorld() )return;

	Hand *avatar = static_cast<Hand*>( networkEntity );
	avatar->sendInputToOther( key , val );
}

void UNetworkHandComponent::onGetInputAction( uint8 key , uint8 val )
{
	if ( networkEntity == nullptr ) return;
	if ( isLocallyControl )return;
	if ( !networkEntity->inWorld() )return;

	if ( ownerHand )
	{
		if ( key == 0 ) ownerHand->indexFingerValue = val;
		else if ( key == 1 ) ownerHand->middleFingerValue = val;
	}
}

void UNetworkHandComponent::sendHandInputAxis( uint8 key , float val )
{
	if ( networkEntity == nullptr ) return;
	if ( !isLocallyControl )return;
	if ( !networkEntity->inWorld() )return;

	Hand *avatar = static_cast<Hand*>( networkEntity );
	avatar->sendAxisToOther( key , val );
}

void UNetworkHandComponent::onGetInputAxis( uint8 key , float val )
{
	if ( networkEntity == nullptr ) return;
	if ( isLocallyControl )return;
	if ( !networkEntity->inWorld() )return;

	if ( ownerHand )
	{
		if ( key == 2 ) ownerHand->thumbFingerValue = val;
	}
}

void UNetworkHandComponent::sendLaserState( uint8 laser )
{
	if ( networkEntity == nullptr ) return;
	if ( !isLocallyControl )return;
	if ( !networkEntity->inWorld() )return;

	Hand *avatar = static_cast<Hand*>( networkEntity );
	avatar->sendLaserStateToOther( laser );
}

void UNetworkHandComponent::onGetLaserState( uint8 laser )
{
	if ( networkEntity == nullptr ) return;
	if ( isLocallyControl )return;
	if ( !networkEntity->inWorld() )return;

	ALaserActor*laserActor = ownerHand->getLaserActor();
	if ( laserActor )
	{
		laserActor->SetLaserEnable( 1 , true );
		laserActor->SetLaserMode( static_cast<LaserModeType>( laser ) );
	}
}

void UNetworkHandComponent::sendGrabState( uint8 grab )
{
	if ( networkEntity == nullptr ) return;
	if ( !isLocallyControl )return;
	if ( !networkEntity->inWorld() )return;

	Hand *avatar = static_cast<Hand*>( networkEntity );
	avatar->sendGrabStateToOther( grab );
}

void UNetworkHandComponent::onGetGrabState( uint8 grab )
{
	if ( networkEntity == nullptr ) return;
	if ( isLocallyControl )return;
	if ( !networkEntity->inWorld() )return;

	if ( ownerHand )
	{
		ownerHand->handState = static_cast<HandState>( grab );
	}
}
