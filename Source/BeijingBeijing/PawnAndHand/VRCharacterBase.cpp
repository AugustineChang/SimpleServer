// Copyright@2017

#include "VRCharacterBase.h"
#include "VRCameraComponent.h"
#include "Network/NetworkComponent/NetworkAvatarComponent.h"
#include "Voice.h"
#include "AudioDevice.h"
#include "GameControl/GlobalMessageDispatcher.h"
#include "IHeadMountedDisplay.h"
#include "LipSyncMicInputComponent.h"
#include "LipSyncMorphTargetComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "kismet/KismetSystemLibrary.h"
#include "kismet/GameplayStatics.h"
#include "Sound/AudioSettings.h"
#include "Sound/SoundWaveProcedural.h"

// Sets default values
AVRCharacterBase::AVRCharacterBase() : maxRawCaptureDataSize( 256 * 1024 ) , curReceiveVoiceData( 0 ) ,
TurnRightRate( 11.25f ) , HeadMoveSpeed( 0.8f ) , isPressLeftTouch( false ) , MoveSpeed( 0.4f ) ,
turnTimer( 0.0f ) , backDistance( 80.0f )
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize( 20.0f , 88.0f );

	VRCamera = CreateDefaultSubobject<UVRCameraComponent>( TEXT( "VRCamera" ) );
	VRCamera->SetupAttachment( RootComponent );
	VRCamera->SetRelativeLocation( FVector( 0.0f , 0.0f , -90.0f ) );

	voiceAudioComponent = CreateDefaultSubobject<UAudioComponent>( TEXT( "voiceAudio" ) );
	voiceAudioComponent->SetupAttachment( RootComponent );

	lipSyncMicInput = CreateDefaultSubobject<ULipSyncMicInputComponent>( TEXT( "lipSyncMicInput" ) );
	this->AddOwnedComponent( lipSyncMicInput );

	lipSyncMorphTarget = CreateDefaultSubobject<ULipSyncMorphTargetComponent>( TEXT( "lipSyncMorphTarget" ) );
	this->AddOwnedComponent( lipSyncMorphTarget );

	networkPlayer = CreateDefaultSubobject<UNetworkAvatarComponent>( TEXT( "networkPlayer" ) );
	this->AddOwnedComponent( networkPlayer );

	receiveVoiceData.Empty( maxRawCaptureDataSize );
	receiveVoiceData.AddUninitialized( maxRawCaptureDataSize );

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->MaxWalkSpeed = 225.0f;
	GetCharacterMovement()->RotationRate = FRotator( 0.0f , 360.0f , 0.0f );
	GetCharacterMovement()->AirControl = 0.2f;
}

// Called when the game starts or when spawned
void AVRCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	voiceDecoder = FVoiceModule::Get().CreateVoiceDecoder();
	createSoundWave();
	
	if ( ControllerClass.Get() == nullptr )
		ControllerClass = AMotionControllerBase::StaticClass();

	leftController = GetWorld()->SpawnActorDeferred<AMotionControllerBase>( 
		*ControllerClass , FTransform() ,  this , nullptr , ESpawnActorCollisionHandlingMethod::AlwaysSpawn );
	leftController->chirality = EControllerHand::Left;
	UGameplayStatics::FinishSpawningActor( leftController , FTransform() );
	leftController->AttachToComponent( RootComponent , FAttachmentTransformRules::SnapToTargetNotIncludingScale );
	
	rightController = GetWorld()->SpawnActorDeferred<AMotionControllerBase>( 
		*ControllerClass , FTransform() , this , nullptr , ESpawnActorCollisionHandlingMethod::AlwaysSpawn );
	rightController->chirality = EControllerHand::Right;
	UGameplayStatics::FinishSpawningActor( rightController , FTransform() );
	rightController->AttachToComponent( RootComponent , FAttachmentTransformRules::SnapToTargetNotIncludingScale );

	if ( GEngine->HMDDevice.IsValid() )
	{
		isOculus = GEngine->HMDDevice->GetDeviceName() == FName( TEXT( "OculusRift" ) );
		GEngine->HMDDevice->SetTrackingOrigin( EHMDTrackingOrigin::Floor );
	}
	else
	{
		isOculus = true;
	}
}

void AVRCharacterBase::Destroyed()
{
	if ( leftController )
	{
		leftController->Destroy();
	}

	if ( rightController )
	{
		rightController->Destroy();
	}
}

// Called every frame
void AVRCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if ( !networkPlayer->getIsLocalControl() ) return;

	turnTimer += DeltaTime;

	updateCharacterTransform();
	playVoiceData();
}

// Called to bind functionality to input
void AVRCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis( TEXT( "ThumbstickLeftY" ) , this , &AVRCharacterBase::ThumbstickLeft_Y );
	PlayerInputComponent->BindAxis( TEXT( "ThumbstickLeftX" ) , this , &AVRCharacterBase::ThumbstickLeft_X );
	
	PlayerInputComponent->BindAxis( TEXT( "ThumbstickRightY" ) , this , &AVRCharacterBase::ThumbstickRight_Y );
	PlayerInputComponent->BindAxis( TEXT( "ThumbstickRightX" ) , this , &AVRCharacterBase::ThumbstickRight_X );

	PlayerInputComponent->BindAction( TEXT( "PressTouchPad" ) , EInputEvent::IE_Pressed , this , &AVRCharacterBase::ThumbstickLeftPress );
	PlayerInputComponent->BindAction( TEXT( "PressTouchPad" ) , EInputEvent::IE_Released , this , &AVRCharacterBase::ThumbstickLeftRelease );

	PlayerInputComponent->BindAction( TEXT( "TriggerLeft" ) , EInputEvent::IE_Pressed , this , &AVRCharacterBase::TriggerLeftPress );
	PlayerInputComponent->BindAction( TEXT( "TriggerLeft" ) , EInputEvent::IE_Released , this , &AVRCharacterBase::TriggerLeftRelease );

	PlayerInputComponent->BindAction( TEXT( "TriggerRight" ) , EInputEvent::IE_Pressed , this , &AVRCharacterBase::TriggerRightPress );
	PlayerInputComponent->BindAction( TEXT( "TriggerRight" ) , EInputEvent::IE_Released , this , &AVRCharacterBase::TriggerRightRelease );

	PlayerInputComponent->BindAction( TEXT( "GripLeft" ) , EInputEvent::IE_Pressed , this , &AVRCharacterBase::GripLeftPress );
	PlayerInputComponent->BindAction( TEXT( "GripLeft" ) , EInputEvent::IE_Released , this , &AVRCharacterBase::GripLeftRelease );

	PlayerInputComponent->BindAction( TEXT( "GripRight" ) , EInputEvent::IE_Pressed , this , &AVRCharacterBase::GripRightPress );
	PlayerInputComponent->BindAction( TEXT( "GripRight" ) , EInputEvent::IE_Released , this , &AVRCharacterBase::GripRightRelease );

	PlayerInputComponent->BindAction( TEXT( "ThumbstickLeft" ) , EInputEvent::IE_Pressed , this , &AVRCharacterBase::ThumbstickLeftPress );
	PlayerInputComponent->BindAction( TEXT( "ThumbstickLeft" ) , EInputEvent::IE_Released , this , &AVRCharacterBase::ThumbstickLeftRelease );

	PlayerInputComponent->BindAction( TEXT( "ThumbstickRight" ) , EInputEvent::IE_Pressed , this , &AVRCharacterBase::ThumbstickRightPress );
	PlayerInputComponent->BindAction( TEXT( "ThumbstickRight" ) , EInputEvent::IE_Released , this , &AVRCharacterBase::ThumbstickRightRelease );
}

void AVRCharacterBase::MoveForward( float val )
{
	walkSpeed = 0.0f;
	if ( isOculus || isPressLeftTouch )
	{
		float absVal = FMath::Abs( val );
		if ( absVal <= 0.01f ) val = 0.0f;

		if ( val > 0.0f )
		{
			FVector forward = FVector::VectorPlaneProject( VRCamera->GetForwardVector() , FVector::UpVector );
			forward.Normalize();

			AddMovementInput( forward , MoveSpeed );
			walkSpeed = 1.0f;
		}
		else if ( val < 0.0f )
		{
			FVector forward = FVector::VectorPlaneProject( VRCamera->GetForwardVector() , FVector::UpVector );
			forward.Normalize();
			
			AddMovementInput( -forward , MoveSpeed );
			walkSpeed = -1.0f;
		}
	}
}

void AVRCharacterBase::TurnAround( float val )
{
	if ( !isOculus && !isPressLeftTouch ) return;

	if ( val > 0.9f )
	{
		if ( turnTimer >= 0.5f )
		{
			AddControllerYawInput( TurnRightRate );
			turnTimer = 0.0f;
		}
	}
	else if ( val < -0.9f )
	{
		if ( turnTimer >= 0.5f )
		{
			AddControllerYawInput( -TurnRightRate );
			turnTimer = 0.0f;
		}
	}
}

void AVRCharacterBase::PressTouchPad( bool isPress )
{
	isPressLeftTouch = isPress;
}

//Input
//////////////////////////////////////////////////////////////////////////

void AVRCharacterBase::TriggerLeftPress()
{
	bool hasMessager;
	UGlobalMessageDispatcher *messager = UGlobalMessageDispatcher::GetGlobalMessager( hasMessager );
	if ( hasMessager )
	{
		messager->OnLeftTrigger_Pressed.Broadcast();
	}
}

void AVRCharacterBase::TriggerLeftRelease()
{
	bool hasMessager;
	UGlobalMessageDispatcher *messager = UGlobalMessageDispatcher::GetGlobalMessager( hasMessager );
	if ( hasMessager )
	{
		messager->OnLeftTrigger_Released.Broadcast();
	}
}

void AVRCharacterBase::TriggerRightPress()
{
	bool hasMessager;
	UGlobalMessageDispatcher *messager = UGlobalMessageDispatcher::GetGlobalMessager( hasMessager );
	if ( hasMessager )
	{
		messager->OnRightTrigger_Pressed.Broadcast();
	}
}

void AVRCharacterBase::TriggerRightRelease()
{
	bool hasMessager;
	UGlobalMessageDispatcher *messager = UGlobalMessageDispatcher::GetGlobalMessager( hasMessager );
	if ( hasMessager )
	{
		messager->OnRightTrigger_Released.Broadcast();
	}
}

void AVRCharacterBase::GripLeftPress()
{
	bool hasMessager;
	UGlobalMessageDispatcher *messager = UGlobalMessageDispatcher::GetGlobalMessager( hasMessager );
	if ( hasMessager )
	{
		messager->OnLeftGrip_Pressed.Broadcast();
	}
}

void AVRCharacterBase::GripLeftRelease()
{
	bool hasMessager;
	UGlobalMessageDispatcher *messager = UGlobalMessageDispatcher::GetGlobalMessager( hasMessager );
	if ( hasMessager )
	{
		messager->OnLeftGrip_Released.Broadcast();
	}
}

void AVRCharacterBase::GripRightPress()
{
	bool hasMessager;
	UGlobalMessageDispatcher *messager = UGlobalMessageDispatcher::GetGlobalMessager( hasMessager );
	if ( hasMessager )
	{
		messager->OnRightGrip_Pressed.Broadcast();
	}
}

void AVRCharacterBase::GripRightRelease()
{
	bool hasMessager;
	UGlobalMessageDispatcher *messager = UGlobalMessageDispatcher::GetGlobalMessager( hasMessager );
	if ( hasMessager )
	{
		messager->OnRightGrip_Released.Broadcast();
	}
}

void AVRCharacterBase::ThumbstickLeftPress()
{
	PressTouchPad( true );

	bool hasMessager;
	UGlobalMessageDispatcher *messager = UGlobalMessageDispatcher::GetGlobalMessager( hasMessager );
	if ( hasMessager )
	{
		messager->OnLeftThumbstick_Pressed.Broadcast();
	}
}

void AVRCharacterBase::ThumbstickLeftRelease()
{
	PressTouchPad( false );

	bool hasMessager;
	UGlobalMessageDispatcher *messager = UGlobalMessageDispatcher::GetGlobalMessager( hasMessager );
	if ( hasMessager )
	{
		messager->OnLeftThumbstick_Released.Broadcast();
	}
}

void AVRCharacterBase::ThumbstickRightPress()
{
	bool hasMessager;
	UGlobalMessageDispatcher *messager = UGlobalMessageDispatcher::GetGlobalMessager( hasMessager );
	if ( hasMessager )
	{
		messager->OnRightThumbstick_Pressed.Broadcast();
	}
}

void AVRCharacterBase::ThumbstickRightRelease()
{
	bool hasMessager;
	UGlobalMessageDispatcher *messager = UGlobalMessageDispatcher::GetGlobalMessager( hasMessager );
	if ( hasMessager )
	{
		messager->OnRightThumbstick_Released.Broadcast();
	}
}

void AVRCharacterBase::ThumbstickLeft_X( float value )
{
	TurnAround( value );

	bool hasMessager;
	UGlobalMessageDispatcher *messager = UGlobalMessageDispatcher::GetGlobalMessager( hasMessager );
	if ( hasMessager )
	{
		messager->LeftThumbstickAxisX.Broadcast( value );
	}
}

void AVRCharacterBase::ThumbstickLeft_Y( float value )
{
	MoveForward( value );

	bool hasMessager;
	UGlobalMessageDispatcher *messager = UGlobalMessageDispatcher::GetGlobalMessager( hasMessager );
	if ( hasMessager )
	{
		messager->LeftThumbstickAxisY.Broadcast( value );
	}
}

void AVRCharacterBase::ThumbstickRight_X( float value )
{
	bool hasMessager;
	UGlobalMessageDispatcher *messager = UGlobalMessageDispatcher::GetGlobalMessager( hasMessager );
	if ( hasMessager )
	{
		messager->RightThumbstickAxisX.Broadcast( value );
	}
}

void AVRCharacterBase::ThumbstickRight_Y( float value )
{
	bool hasMessager;
	UGlobalMessageDispatcher *messager = UGlobalMessageDispatcher::GetGlobalMessager( hasMessager );
	if ( hasMessager )
	{
		messager->RightThumbstickAxisY.Broadcast( value );
	}
}

//////////////////////////////////////////////////////////////////////////

void AVRCharacterBase::createSoundWave()
{
	USoundWaveProcedural* SoundStreaming = NewObject<USoundWaveProcedural>();
	SoundStreaming->SampleRate = VOICE_SAMPLE_RATE;
	SoundStreaming->NumChannels = 1;
	SoundStreaming->Duration = INDEFINITELY_LOOPING_DURATION;
	SoundStreaming->SoundGroup = SOUNDGROUP_Voice;
	SoundStreaming->bLooping = false;

	voiceAudioComponent->SetSound( SoundStreaming );
}

void AVRCharacterBase::playVoiceData()
{
	if ( curReceiveVoiceData <= 0 ) return;

	if ( voiceAudioComponent )
	{
		if ( !voiceAudioComponent->IsActive() )
		{
			voiceAudioComponent->Play();
		}
		
		USoundWaveProcedural* SoundStreaming = Cast<USoundWaveProcedural>( voiceAudioComponent->Sound );
		if ( SoundStreaming )
		{
			SoundStreaming->QueueAudio( receiveVoiceData.GetData() , curReceiveVoiceData );
			curReceiveVoiceData = 0;
		}
	}
}

void AVRCharacterBase::onReceiveVoiceData( const TArray<uint8> &receiveData )
{
	int32 realCompressedSize = receiveData.Num();
	uint32 realRawDataSize = maxRawCaptureDataSize - curReceiveVoiceData;
	voiceDecoder->Decode( receiveData.GetData() , realCompressedSize , receiveVoiceData.GetData() + curReceiveVoiceData , realRawDataSize );
	curReceiveVoiceData = curReceiveVoiceData + realRawDataSize;
}

void AVRCharacterBase::updateCharacterTransform()
{
	if ( GEngine->HMDDevice.IsValid() )
	{
		FVector newLocation = GetActorTransform().TransformPosition( VRCamera->GetDeltaLocation() * HeadMoveSpeed );
		this->SetActorLocation( newLocation , true );
	}
}
