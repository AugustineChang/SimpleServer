// Copyright 2017-2020 nasuVR All Rights Reserved.

#include "SundialActor.h"
#include "PawnAndHand/MotionControllerBase.h"
#include "PawnAndHand/LimitMotionController.h"
#include "kismet/KismetMathLibrary.h"
#include "GameControl/GlobalMessageDispatcher.h"


// Sets default values
ASundialActor::ASundialActor() : isNearSundial( false ) , isPlaySundialEmissive( false ) ,
isNearPointer( false ) , isPlayPointerEmissive( false ) , isTriggerPressed( false ) , 
sundialHandActor( nullptr ) , pointerHandActor( nullptr )
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	root = CreateDefaultSubobject<USceneComponent>( TEXT( "Root" ) );
	RootComponent = root;

	sundialMesh = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "SundialMesh" ) );
	sundialMesh->SetupAttachment( RootComponent );

	sundialBox = CreateDefaultSubobject<UBoxComponent>( TEXT( "SundialBox" ) );
	sundialBox->SetupAttachment( RootComponent );

	sundialCenter = CreateDefaultSubobject<USceneComponent>( TEXT( "SundialCenter" ) );
	sundialCenter->SetupAttachment( RootComponent );

	sundialRotatePoint = CreateDefaultSubobject<USceneComponent>( TEXT( "RotatePoint" ) );
	sundialRotatePoint->SetupAttachment( sundialCenter );

	pointerMesh = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "PointerMesh" ) );
	pointerMesh->SetupAttachment( sundialRotatePoint );

	pointerBox = CreateDefaultSubobject<UBoxComponent>( TEXT( "PointerBox" ) );
	pointerBox->SetupAttachment( sundialRotatePoint );
}

// Called when the game starts or when spawned
void ASundialActor::BeginPlay()
{
	Super::BeginPlay();
	
	sundialBox->OnComponentBeginOverlap.AddDynamic( this , &ASundialActor::OnApproachSundial );
	sundialBox->OnComponentEndOverlap.AddDynamic( this , &ASundialActor::OnLeaveSundial );
	pointerBox->OnComponentBeginOverlap.AddDynamic( this , &ASundialActor::OnApproachPointer );
	pointerBox->OnComponentEndOverlap.AddDynamic( this , &ASundialActor::OnLeavePointer );

	sundialDynMat = sundialMesh->CreateDynamicMaterialInstance( 1 );
	pointerDynMat = pointerMesh->CreateDynamicMaterialInstance( 0 );
}

// Called every frame
void ASundialActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	updateSundialEmissive( DeltaTime );
	updatePointerEmissive( DeltaTime );

	calcPointerRotation();
}

void ASundialActor::updateSundialEmissive( float deltaTime )
{
	if ( !isPlaySundialEmissive ) return;

	float targetEmissive = isNearSundial ? 10.0f : 0.0f;
	curSundialEmissive = FMath::Lerp( curSundialEmissive , targetEmissive , deltaTime * 5.0f );
	FLinearColor emissiveCol( 0.0f , curSundialEmissive , curSundialEmissive );
	sundialDynMat->SetVectorParameterValue( FName( TEXT( "Emissive HDR" ) ) , emissiveCol );

	if ( FMath::IsNearlyEqual( curSundialEmissive , targetEmissive , 0.001f ) )
	{
		isPlaySundialEmissive = false;
	}
}

void ASundialActor::updatePointerEmissive( float deltaTime )
{
	if ( !isPlayPointerEmissive ) return;

	float targetEmissive = isNearPointer ? 10.0f : ( isNearSundial ? 1.0f : 0.0f );

	curPointerEmissive = FMath::Lerp( curPointerEmissive , targetEmissive , deltaTime * 5.0f );
	FLinearColor emissiveCol( 0.0f , curPointerEmissive , curPointerEmissive );
	pointerDynMat->SetVectorParameterValue( FName( TEXT( "Emissive HDR" ) ) , emissiveCol );

	if ( FMath::IsNearlyEqual( curPointerEmissive , targetEmissive , 0.001f ) )
	{
		isPlayPointerEmissive = false;
	}
}

void ASundialActor::calcPointerRotation()
{
	if ( !isTriggerPressed ) return;
	
	FVector handInCenter = sundialCenter->GetComponentToWorld().InverseTransformPosition( pointerHandActor->GetActorLocation() );
	handInCenter = UKismetMathLibrary::ProjectVectorOnToPlane( handInCenter , FVector::UpVector );
	FRotator relativeRot = UKismetMathLibrary::MakeRotFromYZ( handInCenter , FVector::UpVector );

	float angle = relativeRot.Yaw;
	angle = FMath::FloorToFloat( angle / 30.0f ) * 30.0f + 8.0f;
	relativeRot.Yaw = angle;

	sundialRotatePoint->SetRelativeRotation( relativeRot );
}

void ASundialActor::bindTriggerEventDynamically( bool isBind )
{
	bool hasMessager;
	UGlobalMessageDispatcher *messager = UGlobalMessageDispatcher::GetGlobalMessager( hasMessager );
	if ( hasMessager )
	{
		if ( isBind )
		{
			if ( isBindLeftHand )
			{
				messager->OnLeftTrigger_Pressed.AddDynamic( this , &ASundialActor::OnTriggerPressed );
				messager->OnLeftTrigger_Released.AddDynamic( this , &ASundialActor::OnTriggerReleased );
			}
			else
			{
				messager->OnRightTrigger_Pressed.AddDynamic( this , &ASundialActor::OnTriggerPressed );
				messager->OnRightTrigger_Released.AddDynamic( this , &ASundialActor::OnTriggerReleased );
			}
		}
		else
		{
			if ( isBindLeftHand )
			{
				messager->OnLeftTrigger_Pressed.RemoveDynamic( this , &ASundialActor::OnTriggerPressed );
				messager->OnLeftTrigger_Released.RemoveDynamic( this , &ASundialActor::OnTriggerReleased );
			}
			else
			{
				messager->OnRightTrigger_Pressed.RemoveDynamic( this , &ASundialActor::OnTriggerPressed );
				messager->OnRightTrigger_Released.RemoveDynamic( this , &ASundialActor::OnTriggerReleased );
			}
		}
	}
}

void ASundialActor::OnTriggerPressed()
{
	isTriggerPressed = true;
}

void ASundialActor::OnTriggerReleased()
{
	isTriggerPressed = false;

	if ( !savedNearPointer )
	{
		isNearPointer = false;
		isPlayPointerEmissive = true;

		pointerHandActor = nullptr;
		bindTriggerEventDynamically( false );
	}

	if ( !savedNearSundial )
	{
		isNearSundial = false;
		isPlaySundialEmissive = true;
		isPlayPointerEmissive = true;

		sundialHandActor = nullptr;
	}
}

void ASundialActor::OnApproachSundial( UPrimitiveComponent* OverlappedComp , AActor* OtherActor , UPrimitiveComponent* OtherComp , int32 OtherBodyIndex , bool bFromSweep , const FHitResult& SweepResult )
{
	if ( !Cast<AMotionControllerBase>( OtherActor ) ) return;
	if ( Cast<ALimitMotionController>( OtherActor ) ) return;
	if ( sundialHandActor != nullptr ) return;
	
	if ( !isTriggerPressed )
	{
		isNearSundial = true;
		isPlaySundialEmissive = true;
		isPlayPointerEmissive = true;

		sundialHandActor = OtherActor;
	}
	else
		savedNearSundial = true;
}

void ASundialActor::OnLeaveSundial( UPrimitiveComponent* OverlappedComp , AActor* OtherActor , UPrimitiveComponent* OtherComp , int32 OtherBodyIndex )
{
	if ( !Cast<AMotionControllerBase>( OtherActor ) ) return;
	if ( Cast<ALimitMotionController>( OtherActor ) ) return;
	if ( sundialHandActor != OtherActor ) return;

	if ( !isTriggerPressed )
	{
		isNearSundial = false;
		isPlaySundialEmissive = true;
		isPlayPointerEmissive = true;

		sundialHandActor = nullptr;
	}
	else
		savedNearSundial = false;
}

void ASundialActor::OnApproachPointer( UPrimitiveComponent* OverlappedComp , AActor* OtherActor , UPrimitiveComponent* OtherComp , int32 OtherBodyIndex , bool bFromSweep , const FHitResult& SweepResult )
{
	AMotionControllerBase *hand = Cast<AMotionControllerBase>( OtherActor );
	if ( !hand ) return;
	if ( Cast<ALimitMotionController>( OtherActor ) ) return;
	if ( pointerHandActor != nullptr ) return;
	
	if ( !isTriggerPressed )
	{
		isNearPointer = true;
		isPlayPointerEmissive = true;

		pointerHandActor = hand;
		isBindLeftHand = hand->chirality == EControllerHand::Left;
		bindTriggerEventDynamically( true );
	}
	else
		savedNearPointer = true;
}

void ASundialActor::OnLeavePointer( UPrimitiveComponent* OverlappedComp , AActor* OtherActor , UPrimitiveComponent* OtherComp , int32 OtherBodyIndex )
{
	if ( !Cast<AMotionControllerBase>( OtherActor ) ) return;
	if ( Cast<ALimitMotionController>( OtherActor ) ) return;
	if ( pointerHandActor != OtherActor ) return;

	if ( !isTriggerPressed )
	{
		isNearPointer = false;
		isPlayPointerEmissive = true;

		pointerHandActor = nullptr;
		bindTriggerEventDynamically( false );
	}
	else
		savedNearPointer = false;
}

