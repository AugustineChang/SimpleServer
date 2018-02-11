// Copyright@2017

#include "NetworkEntityComponent.h"
#include "Network/KBE_Entity/LogicEvent.h"
#include "KBEngine.h"

// Sets default values for this component's properties
UNetworkEntityComponent::UNetworkEntityComponent( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer ) , lerpFactor( 5.0f ) , slerpFactor( 8.0f ) , intervalTime( 0.03f ) ,
	componentsNum( 0 )
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UNetworkEntityComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UNetworkEntityComponent::lerpTransform( int32 index , float DeltaTime )
{
	USceneComponent *targetComp = targetSceneComponent[index];
	if ( targetComp == nullptr ) return;

	if ( !isReachTarget[index] )
	{
		FVector targetLoc = targetLocation[index];
		FQuat targetRot = targetRotation[index];

		bool isReach = false;
		if ( isSyncRelative[index] )
		{
			const FTransform &trans = targetComp->GetRelativeTransform();

			FVector curLoc = trans.GetLocation();
			FVector lerpLoc = FMath::Lerp<FVector>( curLoc , targetLoc , DeltaTime * lerpFactor );

			isReach = targetLoc.Equals( lerpLoc , 1.0f );

			FQuat curRot = trans.GetRotation();
			FQuat lerpRot = FQuat::Slerp( curRot , targetRot , DeltaTime * slerpFactor );

			isReach = isReach && targetRot.Equals( lerpRot , 0.01f );

			targetComp->SetRelativeLocation( lerpLoc );
			targetComp->SetRelativeRotation( lerpRot );
		}
		else
		{
			FVector curLoc = targetComp->GetComponentLocation();
			FVector lerpLoc = FMath::Lerp<FVector>( curLoc , targetLoc , DeltaTime * lerpFactor );

			isReach = targetLoc.Equals( lerpLoc , 1.0f );

			FQuat curRot = targetComp->GetComponentQuat();
			FQuat lerpRot = FQuat::Slerp( curRot , targetRot , DeltaTime * slerpFactor );

			isReach = isReach && targetRot.Equals( lerpRot , 0.03f );

			targetComp->SetWorldLocationAndRotation( lerpLoc , lerpRot );
		}

		isReachTarget[index] = isReach;
	}
}

// Called every frame
void UNetworkEntityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	for ( int32 i = 0; i < componentsNum; ++i )
	{
		lerpTransform( i , DeltaTime );
	}
}

void UNetworkEntityComponent::setupEntity( USceneComponent * targetComp , bool isRelative )
{
	targetSceneComponent.Add( targetComp );
	isSyncRelative.Add( isRelative );

	isPausePosUpdate.Add( false );
	isPauseRotUpdate.Add( false );

	targetLocation.Add( FVector::ZeroVector );
	targetRotation.Add( FQuat::Identity );
	isReachTarget.Add( true );

	lastSendLocation.Add( FVector::ZeroVector );
	lastSendRotation.Add( FRotator::ZeroRotator );
	lastSendTimer.Add( 0.0f );

	++componentsNum;
}

void UNetworkEntityComponent::setUpdatePause( int32 index , bool isPausePos , bool isPauseRot )
{
	isPausePosUpdate[index] = isPausePos;
	isPauseRotUpdate[index] = isPauseRot;
	
	USceneComponent *targetComp = targetSceneComponent[index];
	if ( targetComp == nullptr ) return;

	if ( !isSyncRelative[index] )
	{
		const FTransform &trans = targetComp->GetComponentTransform();

		if( isPausePosUpdate[index] ) targetLocation[index] = trans.GetLocation();
		if( isPauseRotUpdate[index] ) targetRotation[index] = trans.GetRotation();
	}
	else
	{
		const FTransform &trans = targetComp->GetRelativeTransform();
		if ( isPausePosUpdate[index] ) targetLocation[index] = trans.GetLocation();
		if ( isPauseRotUpdate[index] ) targetRotation[index] = trans.GetRotation();
	}
}

void UNetworkEntityComponent::checkSendTrans( int32 index , FVector &position , FRotator &rotation , bool &isPosValid , bool &isRotValid )
{
	isPosValid = false;
	isRotValid = false;

	USceneComponent *targetComp = targetSceneComponent[index];
	if ( targetComp == nullptr ) return;

	if ( !getHasNetwork() )return;
	if ( !isLocallyControl ) return;
	if ( !networkEntity->inWorld() ) return;

	//force send interval 0.025
	float curTime = GetWorld()->TimeSeconds;
	float timeDiff = curTime - lastSendTimer[index];
	if ( timeDiff < intervalTime ) return;
	lastSendTimer[index] = curTime;

	if ( isSyncRelative[index] )
	{
		const FTransform &trans = targetComp->GetRelativeTransform();
		position = trans.GetLocation();
		rotation = trans.Rotator();
	}
	else
	{
		const FTransform &trans = targetComp->GetComponentTransform();
		position = trans.GetLocation();
		rotation = trans.Rotator();
	}

	//有差异 发送
	if ( !isPausePosUpdate[index] && !lastSendLocation[index].Equals( position , 1.0f ) )
	{
		isPosValid = true;
		lastSendLocation[index] = position;
	}

	//有差异 发送
	if ( !isPauseRotUpdate[index] && !lastSendRotation[index].Equals( rotation , 0.03f ) )
	{
		isRotValid = true;
		lastSendRotation[index] = rotation;
	}
}

void UNetworkEntityComponent::onGetPosition( int32 index , const FVector &position )
{
	if ( !getHasNetwork() )return;
	if ( isLocallyControl ) return;
	if ( !networkEntity->inWorld() ) return;
	if ( isPausePosUpdate[index] ) return;

	isReachTarget[index] = false;
	targetLocation[index] = position;
}

void UNetworkEntityComponent::onGetRotation( int32 index , const FRotator &rotation )
{
	if ( !getHasNetwork() )return;
	if ( isLocallyControl ) return;
	if ( !networkEntity->inWorld() ) return;
	if ( isPauseRotUpdate[index] ) return;
	
	isReachTarget[index] = false;
	targetRotation[index] = FQuat( rotation );
}
