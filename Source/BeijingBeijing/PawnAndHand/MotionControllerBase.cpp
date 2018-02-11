// Fill out your copyright notice in the Description page of Project Settings.

#include "MotionControllerBase.h"
#include "VRControllerComponent.h"
#include "VRCharacterBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/ChildActorComponent.h"
#include "Interface/HandInterface.h"
#include "kismet/GameplayStatics.h"
#include "kismet/KismetMathLibrary.h"
#include "IHeadMountedDisplay.h"
#include "GameControl/GlobalMessageDispatcher.h"
#include "Network/NetworkComponent/NetworkHandComponent.h"

// Sets default values
AMotionControllerBase::AMotionControllerBase() : chirality( EControllerHand::Left ) , lastHandProximity( false ) ,
lastNearestObj( nullptr ), handState(HandState::open), ifPressedThumbStick(false) , currentTimeToShow(0.0f),
currentTimeToHide(0.0f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;		

	ControllerComponent = CreateDefaultSubobject<UVRControllerComponent>( TEXT( "MotionController" ) );
	RootComponent = ControllerComponent;

	handMesh = CreateDefaultSubobject<USkeletalMeshComponent>( TEXT( "HandMesh" ) );
	handMesh->SetupAttachment( RootComponent );

	GrabBox = CreateDefaultSubobject<UBoxComponent>( TEXT( "GrabBox" ) );
	GrabBox->SetupAttachment(handMesh);

	TouchSphere = CreateDefaultSubobject<USphereComponent>( TEXT( "TouchSphere" ) );
	TouchSphere->SetupAttachment( handMesh );
	TouchSphere->InitSphereRadius( 1.0f );
	TouchSphere->SetRelativeLocation( FVector::ForwardVector * -2.5f );

	touchWidgetComp = CreateDefaultSubobject<UWidgetInteractionComponent>( TEXT( "TouchWidgetComp" ) );
	touchWidgetComp->SetupAttachment( TouchSphere );
	touchWidgetComp->TraceChannel = UEngineTypes::ConvertToCollisionChannel( TraceTypeQuery4 );
	touchWidgetComp->InteractionDistance = 100.0f;

	laserChildComp = CreateDefaultSubobject<UChildActorComponent>( TEXT( "LaserChildActor" ) );
	laserChildComp->SetupAttachment( RootComponent );
	
	networkHandComp = CreateDefaultSubobject<UNetworkHandComponent>( TEXT( "NetowrkHand" ) );
	this->AddOwnedComponent( networkHandComp );
}

// Called when the game starts or when spawned
void AMotionControllerBase::BeginPlay()
{
	Super::BeginPlay();
	
	if ( chirality == EControllerHand::Left )
	{
		handMesh->SetRelativeScale3D( FVector(1.0f, 1.0f, -1.0f) );		
	}
	else if ( chirality == EControllerHand::Right )
	{
		TouchSphere->OnComponentBeginOverlap.AddDynamic( this , &AMotionControllerBase::OnTouchWidget );
	}
	ControllerComponent->Hand = chirality;
	ControllerComponent->PlayerIndex = 0;

	spawnHandWidget();

	if ( laserClass.Get() != nullptr )
	{
		laserChildComp->SetChildActorClass( laserClass );
	}

	initActionBind();
	updatePlayerIndex( 0 );
}

bool AMotionControllerBase::checkHandProximity()
{
	TArray<AActor*> actors;
	GrabBox->GetOverlappingActors( actors );

	for ( AActor *actor : actors )
	{
		bool hasInterface = actor->GetClass()->ImplementsInterface( UHandInterface::StaticClass() );
		if ( !hasInterface )continue;

		return true;
	}

	return false;
}

void AMotionControllerBase::getNearestObject( AActor *&nearestActor )
{
	TArray<AActor*> actors;
	GrabBox->GetOverlappingActors( actors );

	nearestActor = nullptr;
	float nearestOverlap = 100000000.0f;
	FVector handLocation = GetActorTransform().TransformPosition( FVector( -10.0f , 0.0f , 0.0f ) );

	for ( AActor *actor : actors )
	{
		bool hasInterface = actor->GetClass()->ImplementsInterface( UHandInterface::StaticClass() );
		if ( !hasInterface )continue;

		FVector curLocation = actor->GetActorLocation();
		
		float curDist = FVector::DistSquared( handLocation , curLocation );
		if ( curDist < nearestOverlap )
		{
			nearestOverlap = curDist;
			nearestActor = actor;
		}
	}
}

void AMotionControllerBase::handCheckInterface()
{
	if ( !networkHandComp->getIsLocalControl() ) return;

	if (grabedObject != nullptr) {
		setHandState( HandState::grab );
		return;//已经抓住东西了 不需要检测了
	}		

	bool hasObjs = checkHandProximity();
	if ( hasObjs )
	{
		setHandState( HandState::canGrab );

		AActor *nearestObj;
		getNearestObject( nearestObj );
		
		if ( !lastHandProximity )
		{
			ALaserActor *laser = getLaserActor();
			if ( laser ) 
			{
				LaserModeType oldMode = laser->GetLaserMode();
				laser->SetLaserEnable( 0 , false );
				LaserModeType newMode = laser->GetLaserMode();
				if ( newMode != oldMode )
				{
					networkHandComp->sendLaserState( static_cast<uint8>( newMode ) );
				}
			}
		}

		if ( lastNearestObj != nearestObj )
		{
			if ( lastNearestObj ) IHandInterface::Execute_OnLeave( lastNearestObj , this );
			lastNearestObj = nearestObj;
			if ( nearestObj ) IHandInterface::Execute_OnProximity( nearestObj , this );
		}
	}
	else
	{
		setHandState( HandState::open );

		if ( lastNearestObj ) IHandInterface::Execute_OnLeave( lastNearestObj , this );
		lastNearestObj = nullptr;

		if ( lastHandProximity )
		{
			ALaserActor *laser = getLaserActor();
			if ( laser )
			{
				LaserModeType oldMode = laser->GetLaserMode();
				laser->SetLaserEnable( 0 , true );
				LaserModeType newMode = laser->GetLaserMode();
				if ( newMode != oldMode )
				{
					networkHandComp->sendLaserState( static_cast<uint8>( newMode ) );
				}
			}
		}
	}
	lastHandProximity = hasObjs;
}

void AMotionControllerBase::updatePlayerIndex( int32 playerIndex )
{
	ALaserActor *laser = getLaserActor();
	if ( laser )
	{
		laser->updatePlayerIndex( playerIndex , chirality == EControllerHand::Left );
	}

	if ( touchWidgetComp )
	{
		float offset = chirality == EControllerHand::Left ? 2.0f : 3.0f;
		touchWidgetComp->PointerIndex = playerIndex * 4.0f + offset;
		touchWidgetComp->VirtualUserIndex = playerIndex;
	}
}

void AMotionControllerBase::rumbleController( float intensity )
{
	if ( !networkHandComp->getIsLocalControl() ) return;
	if ( !GEngine->HMDDevice.IsValid() ) return;

	APlayerController *player = UGameplayStatics::GetPlayerController( this , 0 );

	FName hmdName = GEngine->HMDDevice->GetDeviceName();
	if ( hmdName.IsEqual( FName( TEXT( "OculusRift" ) ) ) )
	{
		if ( hapticEffect )
		{
			player->PlayHapticEffect( hapticEffect , chirality , intensity );
		}
	}
	else
	{
		bool isLeft = chirality == EControllerHand::Left;
		FLatentActionInfo latenInfo;
		player->PlayDynamicForceFeedback( intensity , 0.04f , isLeft , isLeft , !isLeft , !isLeft , EDynamicForceFeedbackAction::Start , latenInfo );
	}
}

ALaserActor * AMotionControllerBase::getLaserActor()
{
	if ( laserActor == nullptr )
	{
		laserActor = Cast<ALaserActor>( laserChildComp->GetChildActor() );
	}
	return laserActor;
}

void AMotionControllerBase::initActionBind()
{
	bool hasMessager;
	UGlobalMessageDispatcher *messager = UGlobalMessageDispatcher::GetGlobalMessager( hasMessager );
	if ( !hasMessager ) return;

	if ( this->chirality == EControllerHand::Left )
	{
		messager->OnLeftTrigger_Pressed.AddDynamic( this , &AMotionControllerBase::OnTriggerPressed );
		messager->OnLeftTrigger_Released.AddDynamic( this , &AMotionControllerBase::OnTriggerReleased );
		messager->OnLeftGrip_Pressed.AddDynamic( this , &AMotionControllerBase::OnGripPressed );
		messager->OnLeftGrip_Released.AddDynamic(this, &AMotionControllerBase::OnGripReleased);
		messager->OnLeftThumbstick_Pressed.AddDynamic(this, &AMotionControllerBase::OnThumbStickPressed);
		messager->OnLeftThumbstick_Released.AddDynamic(this, &AMotionControllerBase::OnThumbStickReleased);

		messager->LeftThumbstickAxisX.AddDynamic(this, &AMotionControllerBase::ThumbstickTouch);
		messager->LeftThumbstickAxisY.AddDynamic(this, &AMotionControllerBase::ThumbstickTouch );
	}
	else if ( this->chirality == EControllerHand::Right )
	{
		messager->OnRightTrigger_Pressed.AddDynamic( this , &AMotionControllerBase::OnTriggerPressed );
		messager->OnRightTrigger_Released.AddDynamic( this , &AMotionControllerBase::OnTriggerReleased );
		messager->OnRightGrip_Pressed.AddDynamic( this , &AMotionControllerBase::OnGripPressed );
		messager->OnRightGrip_Released.AddDynamic(this, &AMotionControllerBase::OnGripReleased);
		messager->OnRightThumbstick_Pressed.AddDynamic(this, &AMotionControllerBase::OnThumbStickPressed);
		messager->OnRightThumbstick_Released.AddDynamic(this, &AMotionControllerBase::OnThumbStickReleased);

		messager->RightThumbstickAxisX.AddDynamic(this, &AMotionControllerBase::ThumbstickTouch );
		messager->RightThumbstickAxisY.AddDynamic(this, &AMotionControllerBase::ThumbstickTouch );
	}
}

void AMotionControllerBase::OnTriggerPressed()
{
	if ( lastNearestObj != nullptr )
	{
		IHandInterface::Execute_OnLeave( lastNearestObj , this );
		IHandInterface::Execute_OnPickup( lastNearestObj , this );
		grabedObject = lastNearestObj;		
	}
	else 
	{
		if (handState != HandState::grab) 
		{
			indexFingerValue = 1.0f;
		}
	}

	//key == 0-trigger val == 1 true
	networkHandComp->sendHandInputAction( 0 , 1 );
}

void AMotionControllerBase::OnTriggerReleased()
{
	if ( grabedObject != nullptr )
	{
		IHandInterface::Execute_OnDrop( grabedObject , this );
		grabedObject = nullptr;
		handState = HandState::canGrab;
	}

	if ( lastNearestObj != nullptr )
	{
		IHandInterface::Execute_OnProximity( lastNearestObj , this );
	}

	if (handState != HandState::grab) {
		indexFingerValue = 0.0f;
	}

	//key == 0-trigger val == 0 false
	networkHandComp->sendHandInputAction( 0 , 0 );
}

void AMotionControllerBase::OnGripPressed()
{
	ALaserActor *laser = getLaserActor();
	if ( laser && laser->GetLaserEnable() )
	{
		LaserModeType curMode = laser->GetLaserMode();
		if ( curMode == LaserModeType::Always )
		{
			laser->SetLaserMode( LaserModeType::Close );
			networkHandComp->sendLaserState( static_cast<uint8>( laser->GetLaserMode() ) );
		}
		else if ( curMode == LaserModeType::Close )
		{
			laser->SetLaserMode( LaserModeType::Always );
			networkHandComp->sendLaserState( static_cast<uint8>( laser->GetLaserMode() ) );
		}
	}
	
	if (handState != HandState::grab) {
		middleFingerValue = 1.0f;
	}

	//key == 1-grip val == 1 true
	networkHandComp->sendHandInputAction( 1 , 1 );
}

void AMotionControllerBase::OnGripReleased()
{	
	if (handState != HandState::grab) {
		middleFingerValue = 0.0f;
	}

	//key == 1-grip val == 0 false
	networkHandComp->sendHandInputAction( 1 , 0 );
}

void AMotionControllerBase::OnThumbStickPressed()
{	
	ifPressedThumbStick = true;
	if ( handState != HandState::grab )
	{
		if ( thumbFingerValue != 1.0f )
		{
			//key == 2-thumbstick val == thumbFingerValue
			networkHandComp->sendHandInputAxis( 2 , 1.0f );
		}

		thumbFingerValue = 1.0f;
	}
}

void AMotionControllerBase::OnThumbStickReleased()
{	
	ifPressedThumbStick = false;
	if ( handState != HandState::grab )
	{
		if ( thumbFingerValue != 0.0f )
		{
			//key == 2-thumbstick val == thumbFingerValue
			networkHandComp->sendHandInputAxis( 2 , 0.0f );
		}

		thumbFingerValue = 0.0f;
	}
}

void AMotionControllerBase::ThumbstickTouch( float value )
{
	if ( handState != HandState::grab && !ifPressedThumbStick )
	{
		if ( value != 0.0f )
		{
			if ( thumbFingerValue != 0.5f )
			{
				//key == 1-grip val == thumbFingerValue
				networkHandComp->sendHandInputAxis( 2 , 0.5f );
			}

			thumbFingerValue = 0.5f;
		}
		else
		{
			if ( thumbFingerValue != 0.0f )
			{
				//key == 1-grip val == thumbFingerValue
				networkHandComp->sendHandInputAxis( 2 , 0.0f );
			}

			thumbFingerValue = 0.0f;
		}
	}
}

void AMotionControllerBase::OnTouchWidget( UPrimitiveComponent* OverlappedComp , AActor* OtherActor , UPrimitiveComponent* OtherComp , int32 OtherBodyIndex , bool bFromSweep , const FHitResult& SweepResult )
{
	if ( !touchWidgetComp ) return;
	if ( !Cast<ASettingWidgetActor>( OtherActor ) ) return;

	touchWidgetComp->PressPointerKey( EKeys::LeftMouseButton );
	touchWidgetComp->ReleasePointerKey( EKeys::LeftMouseButton );

	rumbleController( 0.7f );
}

// Called every frame
void AMotionControllerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	handCheckInterface();
	updateTouchWidgetRotation( DeltaTime );
}

void AMotionControllerBase::setHandState( HandState newState )
{
	if ( newState != handState )
	{
		networkHandComp->sendGrabState( static_cast<uint8>( newState ) );
	}
	handState = newState;
}

void AMotionControllerBase::spawnHandWidget()
{
	if ( settingWidgetActorClass.Get() == nullptr )
	{
		TouchSphere->SetCollisionProfileName( FName( "NoCollision" ) );
		touchWidgetComp->DestroyComponent( true );
		touchWidgetComp = nullptr;
		return;
	}

	if ( chirality == EControllerHand::Left )
	{
		FActorSpawnParameters spawnInfo;
		spawnInfo.Owner = this;
		spawnInfo.Instigator = nullptr;
		spawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FTransform spawnTrans;
		spawnTrans.SetRotation( FQuat::MakeFromEuler( FVector::UpVector*90.0f ) );
		spawnTrans.SetLocation( FVector::ForwardVector * -15.0f );

		AActor *spawnedActor = GetWorld()->SpawnActor( *settingWidgetActorClass , &spawnTrans , spawnInfo );
		settingWidgetActor = Cast<ASettingWidgetActor>( spawnedActor );
		settingWidgetActor->AttachToComponent( RootComponent , FAttachmentTransformRules::KeepRelativeTransform );

		TouchSphere->SetCollisionProfileName( FName( "NoCollision" ) );
		touchWidgetComp->DestroyComponent( true );
		touchWidgetComp = nullptr;
	}
	else if ( chirality == EControllerHand::Right )
	{
		TouchSphere->SetCollisionProfileName( FName( "TouchPreset" ) );
		TouchSphere->AttachToComponent( handMesh , FAttachmentTransformRules::KeepRelativeTransform , FName( TEXT( "index_03_r" ) ) );

		AVRCharacterBase *pawn = Cast<AVRCharacterBase>( GetOwner() );
		settingWidgetActor = pawn->leftController->settingWidgetActor;
	}
}

void AMotionControllerBase::updateTouchWidgetRotation( float deltaTime )
{
	if ( !touchWidgetComp ) return;
	if ( !settingWidgetActor ) return;

	FVector widgetNor = settingWidgetActor->getWidgetNormal();
	widgetNor = TouchSphere->GetComponentToWorld().InverseTransformVector( widgetNor );
	touchWidgetComp->SetRelativeRotation( UKismetMathLibrary::MakeRotFromX( -widgetNor ) );
}

