// Copyright@2017

#include "BP_Util.h"
#include "KBEvent.h"
#include "AudioDevice.h"
#include "Sound/SoundWave.h"
#include "Developer/TargetPlatform/Public/Interfaces/IAudioFormat.h"
#include "VorbisAudioInfo.h"
#include "PawnAndHand/VRCharacterBase.h"
#include "GameControl/GlobalMessageDispatcher.h"
#include "GameControl/VRGameInstance.h"
#include "kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

int32 UBP_Util::SetFlag(int32 flag, int32 oriFlag, bool enable)
{
	return ((~flag)&oriFlag) | (enable? flag : 0);
}

void UBP_Util::fireNetworkEvent( const FString &eventName , UKBEventData *pEventData )
{
	KBENGINE_EVENT_FIRE( eventName , pEventData );
}

void UBP_Util::AdjustGlobalSoundVolume(const UObject *WorldContextObject , float InTransientMasterVolume)
{
	if (GEngine) {
		UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject);
		if (World) {
			if (FAudioDevice *audioDevice = World->GetAudioDevice()) {
				audioDevice->SetTransientMasterVolume(InTransientMasterVolume);
			}
		}
	}
}

USoundWave * UBP_Util::GetSoundWaveFromFile(const FString & filePath)
{
	USoundWave* sw = NewObject<USoundWave>();

	if ( !sw )
		return NULL;

	//* If true the song was successfully loaded
	bool loaded = false;

	//* loaded song file (binary, encoded)
	TArray < uint8 > rawFile;

	loaded = FFileHelper::LoadFileToArray( rawFile , filePath.GetCharArray().GetData() );

	if ( loaded )
	{
		FByteBulkData* bulkData = &sw->CompressedFormatData.GetFormat( TEXT( "OGG" ) );

		bulkData->Lock( LOCK_READ_WRITE );
		FMemory::Memcpy( bulkData->Realloc( rawFile.Num() ) , rawFile.GetData() , rawFile.Num() );
		bulkData->Unlock();

		loaded = fillSoundWaveInfo( sw , &rawFile ) == 0 ? true : false;
	}

	if ( !loaded )
		return NULL;

	return sw;
}

FString UBP_Util::getCurTimeString( const FString &format )
{
	return UKismetMathLibrary::Now().ToString( *format );
}

void UBP_Util::networkOpenLevel( AVRCharacterBase *playerPawn , const FString &levelName , bool isBackToHall )
{
	if ( playerPawn == nullptr ) return;

	UVRGameInstance *gameInstance = Cast<UVRGameInstance>( playerPawn->GetGameInstance() );
	if ( playerPawn->networkPlayer->getHasNetwork() )
	{
		bool isLocal = playerPawn->networkPlayer->getIsLocalControl();
		bool isGuide = playerPawn->networkPlayer->IsAvatarGuide();
		if ( !isLocal || !isGuide ) return;

		bool hasMessager;
		UGlobalMessageDispatcher *messager = UGlobalMessageDispatcher::GetGlobalMessager( hasMessager );
		if ( hasMessager )
		{
			messager->DoCustomAction.Broadcast( TEXT( "OpenLevel" ) , levelName , true );
		}
		
		gameInstance->isOpenLevel = true;
		playerPawn->networkPlayer->logoutSpace();

		UGameplayStatics::OpenLevel( playerPawn , FName( *levelName ) );
	}
	else
	{
		UGameplayStatics::OpenLevel( playerPawn , FName( *levelName ) );
	}
	
	if ( isBackToHall )
	{
		gameInstance->clearSpotListName();
	}
}

void UBP_Util::networkQuitGame( class AVRCharacterBase *playerPawn , const FString &offlineLevel )
{
	if ( playerPawn == nullptr ) return;

	if ( playerPawn->networkPlayer->getHasNetwork() )
	{
		bool isLocal = playerPawn->networkPlayer->getIsLocalControl();
		bool isGuide = playerPawn->networkPlayer->IsAvatarGuide();
		if ( !isLocal || !isGuide ) return;

		bool hasMessager;
		UGlobalMessageDispatcher *messager = UGlobalMessageDispatcher::GetGlobalMessager( hasMessager );
		if ( hasMessager )
		{
			messager->DoCustomAction.Broadcast( TEXT( "LogoutGame" ) , TEXT("") , true );
		}

		playerPawn->networkPlayer->logoutGame();

		UGameplayStatics::OpenLevel( playerPawn , FName( *offlineLevel ) );
	}
	else
	{
		UKismetSystemLibrary::QuitGame( playerPawn , Cast<APlayerController>( playerPawn->GetController() ) , EQuitPreference::Quit );
	}

	UVRGameInstance *gameInstance = Cast<UVRGameInstance>( playerPawn->GetGameInstance() );
	gameInstance->clearSpotListName();
}

int UBP_Util::fillSoundWaveInfo( USoundWave* sw , TArray<uint8>* rawFile )
{
	FSoundQualityInfo info;
	FVorbisAudioInfo vorbis_obj;
	if ( !vorbis_obj.ReadCompressedInfo( rawFile->GetData() , rawFile->Num() , &info ) )
	{
		//Debug("Can't load header");
		return 1;
	}

	sw->SoundGroup = ESoundGroup::SOUNDGROUP_Default;
	sw->NumChannels = info.NumChannels;
	sw->Duration = info.Duration;
	sw->RawPCMDataSize = info.SampleDataSize;
	sw->SampleRate = info.SampleRate;

	return 0;
}

