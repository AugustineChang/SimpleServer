// Copyright 2017-2020 nasuVR All Rights Reserved.

#include "Robot.h"
#include "Tasks/AITask_MoveTo.h"
#include "Kismet/GameplayStatics.h"
#include "kismet/KismetMathLibrary.h"
#include "GameControl/GlobalMessageDispatcher.h"
#include "Other/BP_Util.h"
#include "Sound/SoundWave.h"
#include "components/AudioComponent.h"

// Sets default values
ARobot::ARobot() : bIsMovingToTar(false), maxWaitDistance(300.0f) , soundRoot( TEXT( "StreamAssets/Sounds/" ) ) ,
isSelectCell( false )
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	soundRoot = FPaths::Combine( *FPaths::GameContentDir() , *soundRoot );

	audioPlayer = CreateDefaultSubobject<UAudioComponent>( TEXT( "AudioPlayer" ) );
	audioPlayer->SetupAttachment( RootComponent );
}

// Called when the game starts or when spawned
void ARobot::BeginPlay()
{
	Super::BeginPlay();
	
	bool hasMessager;
	UGlobalMessageDispatcher *messager = UGlobalMessageDispatcher::GetGlobalMessager( hasMessager );
	if ( hasMessager )
	{
		messager->DoCustomAction.AddDynamic( this , &ARobot::OnHoverScenicSpot );
		messager->DoCustomAction.AddDynamic( this , &ARobot::OnClickBigScreenCell );
		messager->DoCustomAction.AddDynamic( this , &ARobot::OnPlayVoice );
	}
}

// Called every frame
void ARobot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	LookAtPlayer();
}

bool ARobot::CheckIfCloseToTarget()
{
	float tempDistance = GetDistanceTo(GetOwner());
	if (tempDistance > maxWaitDistance) {
		if (tempDistance > 2 * maxWaitDistance) {
			this->SetActorLocation(GetOwner()->GetActorLocation());
		}
		return true;
	}
	return false;
}

void ARobot::LookAtPlayer()
{
	ACharacter* character = UGameplayStatics::GetPlayerCharacter(this, 0);
	if ( character )
	{
		FRotator tempRot;
		tempRot.Roll = this->GetActorRotation().Roll;
		tempRot.Pitch = this->GetActorRotation().Pitch;
		tempRot.Yaw = UKismetMathLibrary::MakeRotFromX( character->GetActorLocation() - this->GetActorLocation() ).Yaw;

		this->SetActorRotation( tempRot );
	}
}

void ARobot::playSoundByKey( const FString &tableName , const FString &rowKey )
{
	FString soundName;
	UVRGameInstance *gamInstance = Cast<UVRGameInstance>( GetGameInstance() );
	bool ret = gamInstance->getTableData( tableName , rowKey , TEXT( "soundPath" ) , soundName );
	if ( !ret ) return;

	playSoundByName( soundName );
}

void ARobot::playSoundByName( const FString &fileName )
{
	if ( audioPlayer->IsPlaying() )
	{
		audioPlayer->Stop();
	}

	USoundWave **sound = soundMap.Find( fileName );
	if ( !sound )
	{
		USoundWave * loadSound = UBP_Util::GetSoundWaveFromFile( FPaths::Combine( *soundRoot , *fileName ) );
		soundMap.Add( fileName , loadSound );

		audioPlayer->SetSound( loadSound );
		audioPlayer->Play();
	}
	else
	{
		audioPlayer->SetSound( *sound );
		audioPlayer->Play();
	}
}

void ARobot::OnHoverScenicSpot( const FString &cmd , const FString &paramStr , bool isNetworkSend )
{
	if ( isSelectCell ) return;
	if ( cmd != TEXT( "SandBoxHover" ) ) return;

	FString spotKey = paramStr;
	if ( spotKey == TEXT( "NoKey" ) )//unhover
	{
		audioPlayer->Stop();
	}
	else//hover
	{
		playSoundByKey( TEXT( "ScenicSpotTable" ) , spotKey );
	}
}

void ARobot::OnClickBigScreenCell( const FString &cmd , const FString &paramStr , bool isNetworkSend )
{
	if ( cmd != TEXT( "BigScreenClick" ) ) return;

	FString screenKey = paramStr;
	if ( screenKey == TEXT( "NoKey" ) )//cancel click
	{
		isSelectCell = false;
		audioPlayer->Stop();
	}
	else//click
	{
		isSelectCell = true;
		playSoundByKey( TEXT( "BigScreenTable" ) , screenKey );
	}
}

void ARobot::OnPlayVoice( const FString &cmd , const FString &paramStr , bool isNetworkSend )
{
	if ( cmd != TEXT( "PlayVoice" ) ) return;

	playSoundByName( paramStr );
}
