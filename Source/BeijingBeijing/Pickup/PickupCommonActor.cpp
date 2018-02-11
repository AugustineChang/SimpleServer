// Copyright 2017-2020 nasuVR All Rights Reserved.

#include "PickupCommonActor.h"
#include "components/StaticMeshComponent.h"
#include "PawnAndHand/MotionControllerBase.h"
#include "Other/PPGlobalComponent.h"

// Sets default values
APickupCommonActor::APickupCommonActor() : initialResponse( ECollisionResponse::ECR_Ignore )
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	pickupMesh = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "PickupMesh" ) );
	RootComponent = pickupMesh;

	ppGlobal = CreateDefaultSubobject<UPPGlobalComponent>( TEXT( "PostProcess" ) );
	this->AddOwnedComponent( ppGlobal );
}

// Called when the game starts or when spawned
void APickupCommonActor::BeginPlay()
{
	Super::BeginPlay();
	
	initialResponse = pickupMesh->GetCollisionResponseToChannel( ECollisionChannel::ECC_Pawn );
	savedSimulatePhysics = pickupMesh->IsSimulatingPhysics();
}

void APickupCommonActor::initOriginData()
{
	DetachFromActor( FDetachmentTransformRules::KeepWorldTransform );

	FVector PointA = firstHand->GetComponentLocation();
	FVector PointB = secondHand->GetComponentLocation();

	originVector = PointB - PointA;
	originRotation = GetActorRotation();

	originLocRatio = GetPointProjectionOnSegment( PointA , PointB , GetActorLocation() );
	originLocOffset = GetActorLocation() - GetPointOnSegment( PointA , PointB , originLocRatio );
}

float APickupCommonActor::GetPointProjectionOnSegment( const FVector &PointA , const FVector &PointB , const FVector &Point )
{
	FVector lineNorm = ( PointB - PointA );
	float A2B_Length = FVector::Distance( PointA , PointB );
	if ( A2B_Length < 0.0001f )	return 0.0f;
	lineNorm /= A2B_Length;

	FVector A2P = Point - PointA;

	float A2Proj_Length = FVector::DotProduct( A2P , lineNorm );
	return A2Proj_Length / A2B_Length;
}

FVector APickupCommonActor::GetPointOnSegment( const FVector &PointA , const FVector &PointB , float Factor )
{
	if ( Factor == 0.0f ) return FVector::ZeroVector;

	return Factor * PointB + ( 1 - Factor ) * PointA;
}

void APickupCommonActor::UpdateLocationAndRotation()
{
	if ( firstHand == nullptr || secondHand == nullptr ) return;

	FVector PointA = firstHand->GetComponentLocation();
	FVector PointB = secondHand->GetComponentLocation();

	//move with two hands
	if ( originLocRatio != 0.0f )
	{
		FVector newLoc = GetPointOnSegment( PointA , PointB , originLocRatio );
		SetActorLocation( newLoc + originLocOffset );
	}

	//rotate with two hands
	FVector newVector = PointB - PointA;
	FRotator handRot = FQuat::FindBetween( originVector , newVector ).Rotator();
	SetActorRotation( originRotation + handRot );
}

// Called every frame
void APickupCommonActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateLocationAndRotation();
}

void APickupCommonActor::OnPickup_Implementation( AMotionControllerBase *hand )
{
	if ( hand == nullptr ) return;

	if ( firstHand == nullptr && secondHand == nullptr )
	{
		pickupMesh->SetSimulatePhysics( false );
		pickupMesh->SetCollisionResponseToChannel( ECollisionChannel::ECC_Pawn , ECollisionResponse::ECR_Ignore );

		firstHand = hand->GetRootComponent();
		AttachToComponent( firstHand , FAttachmentTransformRules::KeepWorldTransform );
	}
	else if ( firstHand != nullptr && secondHand == nullptr )
	{
		secondHand = hand->GetRootComponent();
	}
	else if ( firstHand == nullptr && secondHand != nullptr )
	{
		firstHand = hand->GetRootComponent();
	}
	else
		return;

	if ( firstHand != nullptr && secondHand != nullptr )
	{
		initOriginData();
	}
}

void APickupCommonActor::OnDrop_Implementation( AMotionControllerBase *hand )
{
	if ( hand == nullptr )return;

	if ( hand->GetRootComponent() == firstHand )
	{
		firstHand = nullptr;
	}
	else if ( hand->GetRootComponent() == secondHand )
	{
		secondHand = nullptr;
	}

	if ( firstHand == nullptr && secondHand == nullptr )
	{
		pickupMesh->SetSimulatePhysics( savedSimulatePhysics );
		pickupMesh->SetCollisionResponseToChannel( ECollisionChannel::ECC_Pawn , initialResponse );

		DetachFromActor( FDetachmentTransformRules::KeepWorldTransform );
	}
	else if ( firstHand != nullptr && secondHand == nullptr )
	{
		AttachToComponent( firstHand , FAttachmentTransformRules::KeepWorldTransform );
	}
	else if ( firstHand == nullptr && secondHand != nullptr )
	{
		AttachToComponent( secondHand , FAttachmentTransformRules::KeepWorldTransform );
	}
}

void APickupCommonActor::OnProximity_Implementation( AMotionControllerBase *hand )
{
	ppGlobal->setPPFlag( EPPGlobalFlags::HIGHLIGHT , true );
}

void APickupCommonActor::OnLeave_Implementation( AMotionControllerBase *hand )
{
	ppGlobal->setPPFlag( EPPGlobalFlags::HIGHLIGHT , false );
}

