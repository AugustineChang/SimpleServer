// Copyright 2017-2020 nasuVR All Rights Reserved.

#include "NetworkAvatarComponent.h"
#include "Voice.h"
#include "VoiceCapture.h"
#include "PawnAndHand/VRCharacterBase.h"
#include "GameControl/VRGameInstance.h"
#include "LipSyncMorphTargetComponent.h"
#include "kismet/GameplayStatics.h"

UNetworkAvatarComponent::UNetworkAvatarComponent( const FObjectInitializer& ObjectInitializer )
	:Super( ObjectInitializer ) , pingTimer( 1.0f ) , pingTickInterval( 1.0f ) , ownerPawn( nullptr ) ,
	curPingIndex( 0 ) , isRecvWithinInternal( false ) , isSyncVoice( true ) , isPauseSyncVoice( true ) ,
	maxCompressedDataSize( 128 * 1024 ) , maxRawCaptureDataSize( 256 * 1024 ) , curRawCaptureData( 0 ) ,
	voiceCapture( nullptr ) , lastSyncVoice( false )
{
	PrimaryComponentTick.bCanEverTick = true;
	emptyMorphData = TArray<float>( { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } );
	lastLipMorphData = emptyMorphData;

	rawCaptureData.Empty( maxRawCaptureDataSize );
	rawCaptureData.AddUninitialized( maxRawCaptureDataSize );

	compressedData.Empty( maxCompressedDataSize );
	compressedData.AddUninitialized( maxCompressedDataSize );
}

void UNetworkAvatarComponent::BeginPlay()
{
	Super::BeginPlay();

	voiceCapture = FVoiceModule::Get().CreateVoiceCapture();
	voiceEncoder = FVoiceModule::Get().CreateVoiceEncoder();

	ownerPawn = Cast<AVRCharacterBase>( GetOwner() );
	if ( ownerPawn )
	{
		setupEntity( ownerPawn->GetRootComponent() , false );
		setupEntity( ownerPawn->VRCamera , true );

		KBENGINE_REGISTER_EVENT( "set_position" , set_position );
		KBENGINE_REGISTER_EVENT( "updatePosition" , updatePosition );
		KBENGINE_REGISTER_EVENT( "set_direction" , set_direction );
	}

	// 注册事件
	bool hasMessager;
	UGlobalMessageDispatcher *messager = UGlobalMessageDispatcher::GetGlobalMessager( hasMessager );
	if ( hasMessager )
	{
		messager->DoCustomAction.AddDynamic( this , &UNetworkAvatarComponent::sendCustomCMD );
		messager->OnGetCustomAction.AddDynamic( this , &UNetworkAvatarComponent::onGetCustomCMD );
	}
}

void UNetworkAvatarComponent::EndPlay( const EEndPlayReason::Type endPlayReason )
{
	Super::EndPlay( endPlayReason );

	if ( endPlayReason == EEndPlayReason::EndPlayInEditor ||
		endPlayReason == EEndPlayReason::Quit )
	{
		if ( isLocallyControl ) logoutGame();
	}

	KBENGINE_DEREGISTER_ALL_EVENT();
}

void UNetworkAvatarComponent::TickComponent( float DeltaTime , ELevelTick TickType , FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime , TickType , ThisTickFunction );

	if ( !getHasNetwork() || !isLocallyControl ) return;

	//get LipData
	if ( ownerPawn )
	{
		ownerPawn->lipSyncMorphTarget->GetPhonemeData( lipMorphData );
	}

	calcPingValue( DeltaTime );
	sendCameraTrans( DeltaTime );
	sendCharMoveSpeed();

	autoVoiceControl( DeltaTime );
	sendLipData();
	getVoiceDataFromVoiceCapture();

	//save cur lipData
	lastLipMorphData = lipMorphData;
	lastSyncVoice = getSyncVoice();
}

void UNetworkAvatarComponent::linkToKBEObject( EntityBase * networkEntity , bool isLocal )
{
	Super::linkToKBEObject( networkEntity , isLocal );

	if ( isSyncVoice && voiceCapture.IsValid() )
	{
		voiceCapture->Start();
	}
}

bool UNetworkAvatarComponent::IsAvatarGuide()
{
	if ( networkEntity == nullptr ) return true;

	Avatar *avatar = static_cast<Avatar*>( networkEntity );
	return avatar->authority == 1;
}

///////////////////////////////////Login/Logout//////////////////////////////////
void UNetworkAvatarComponent::loginToSpace( uint8 mapKey )
{
	if ( networkEntity == nullptr ) return;

	Avatar *avatar = static_cast<Avatar*>( networkEntity );
	avatar->loginToSpace( mapKey );
}

void UNetworkAvatarComponent::logoutSpace()
{
	if ( networkEntity == nullptr ) return;

	Avatar *avatar = static_cast<Avatar*>( networkEntity );
	avatar->logoutSpace();
}

void UNetworkAvatarComponent::logoutGame()
{
	if ( networkEntity == nullptr ) return;

	Avatar *avatar = static_cast<Avatar*>( networkEntity );
	avatar->logoutGame();
}

//////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////Ping///////////////////////////////////////////
void UNetworkAvatarComponent::calcPingValue( float deltaTime )
{
	pingTimer += deltaTime;
	if ( pingTimer >= pingTickInterval )
	{
		if ( !isRecvWithinInternal )
		{
			sendPingValue( 1000 );//超时
		}

		pingTimer -= pingTickInterval;
		pingSendTime = pingTimer;
		curPingIndex++;
		isRecvWithinInternal = false;

		sendPingCall( curPingIndex );
	}
}

void UNetworkAvatarComponent::sendPingCall( int32 pingIndex )
{
	Avatar *avatar = static_cast<Avatar*>( networkEntity );
	avatar->sendPingCall( pingIndex );
}

void UNetworkAvatarComponent::recvPingCall( int32 pingIndex )
{
	if ( pingIndex != curPingIndex )return;//忽略超时消息

	isRecvWithinInternal = true;
	sendPingValue( FMath::FloorToInt( pingTimer - pingSendTime ) * 1000 );
}

void UNetworkAvatarComponent::sendPingValue( int32 ping )
{
	Avatar *avatar = static_cast<Avatar*>( networkEntity );
	avatar->sendPingValue( ping );
}

int32 UNetworkAvatarComponent::getPingValue()
{
	if ( !getHasNetwork() || !isLocallyControl ) return -1;
	else
	{
		Avatar *avatar = static_cast<Avatar*>( networkEntity );
		return avatar->pingValue;
	}

}
////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////Camera///////////////////////////////////////////
void UNetworkAvatarComponent::sendCameraTrans( float deltaTime )
{
	FVector newLoc;
	FRotator newRot;
	bool isPosValid;
	bool isRotValid;

	checkSendTrans( 1 , newLoc , newRot , isPosValid , isRotValid );

	if ( isPosValid ) sendCameraPos( newLoc );
	if ( isRotValid ) sendCameraRot( newRot );
}

void UNetworkAvatarComponent::sendCameraPos( const FVector &pos )
{
	Avatar *avatar = static_cast<Avatar*>( networkEntity );
	avatar->sendCameraPosToOther( pos );
}

void UNetworkAvatarComponent::sendCameraRot( const FRotator &rot )
{
	Avatar *avatar = static_cast<Avatar*>( networkEntity );
	avatar->sendCameraRotToOther( rot );
}

void UNetworkAvatarComponent::OnGetCameraPos( const FVector &pos )
{
	onGetPosition( 1, pos );
}

void UNetworkAvatarComponent::OnGetCameraRot( const FRotator &rot )
{
	onGetRotation( 1 , rot );
}
////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////Move/////////////////////////////////////////
void UNetworkAvatarComponent::set_position( const UKBEventData *pEventData )
{
	if ( networkEntity == nullptr )return;
	if ( isLocallyControl )return;

	const UKBEventData_set_position *pData = Cast<UKBEventData_set_position>( pEventData );
	if ( pData->entityID != networkEntity->id() ) return;

	AVRCharacterBase *ownerPawn = Cast<AVRCharacterBase>( GetOwner() );
	if ( ownerPawn )
	{
		if ( getIsSyncRelative( 0 ) )
			ownerPawn->SetActorRelativeLocation( pData->position );
		else
			ownerPawn->SetActorLocation( pData->position );
	}
}

void UNetworkAvatarComponent::updatePosition( const UKBEventData *pEventData )
{
	if ( networkEntity == nullptr )return;
	if ( isLocallyControl )return;

	const UKBEventData_updatePosition *pData = Cast<UKBEventData_updatePosition>( pEventData );
	if ( pData->entityID != networkEntity->id() ) return;

	onGetPosition( 0 , pData->position );
}

void UNetworkAvatarComponent::set_direction( const UKBEventData *pEventData )
{
	if ( networkEntity == nullptr )return;
	if ( isLocallyControl )return;

	const UKBEventData_set_direction *pData = Cast<UKBEventData_set_direction>( pEventData );
	if ( pData->entityID != networkEntity->id() ) return;

	FRotator ueRot;
	KBDir2UE4Dir( ueRot , pData->direction );
	onGetRotation( 0 , ueRot );
}

////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////Voice/////////////////////////////////////////
void UNetworkAvatarComponent::autoVoiceControl( float deltaTime )
{
	if ( !voiceCapture.IsValid() )
	{
		isPauseSyncVoice = true;
		return;
	}

	int32 len = lipMorphData.Num();
	int32 len2 = lastLipMorphData.Num();
	if ( len <= 0 || len != len2 )return;

	//check is the same
	bool isSame = true;
	for ( int32 i = 0; i < len; ++i )
	{
		isSame = isSame && ( lipMorphData[i] == lastLipMorphData[i] );
		if ( !isSame ) break;
	}

	if ( isSame )
	{
		if ( !isPauseSyncVoice )
		{
			lipMorphTimer += deltaTime;
			if ( lipMorphTimer >= 0.3f )//持续0.3秒钟 数据相同 停止发送
			{
				lipMorphTimer = -0.3f;
				isPauseSyncVoice = true;
			}
		}
	}
	else
	{
		if ( isPauseSyncVoice )
		{
			isPauseSyncVoice = false;
			lipMorphTimer = 0.0f;
		}
	}
}

void UNetworkAvatarComponent::sendLipData()
{
	if ( !networkEntity->inWorld() ) return;

	if ( getSyncVoice() )
	{
		Avatar *avatar = static_cast<Avatar*>( networkEntity );
		avatar->sendLipDataToOther( lipMorphData );
	}
	else
	{
		if ( lastSyncVoice )
		{
			Avatar *avatar = static_cast<Avatar*>( networkEntity );
			avatar->sendLipDataToOther( emptyMorphData );
		}
	}
}

void UNetworkAvatarComponent::onGetLipData( const TArray<float> &lipData )
{
	if ( networkEntity == nullptr )return;
	if ( isLocallyControl ) return;
	if ( !networkEntity->inWorld() ) return;

	if ( ownerPawn )
	{
		ownerPawn->lipSyncMorphTarget->ReceiveLipData( lipData );
	}
}

void UNetworkAvatarComponent::getVoiceDataFromVoiceCapture()
{
	if ( !getSyncVoice() ) return;
	if ( !networkEntity->inWorld() ) return;

	uint32 newVoiceDataLens = 0;
	EVoiceCaptureState::Type vcState = voiceCapture->GetCaptureState( newVoiceDataLens );
	if ( vcState == EVoiceCaptureState::Ok && newVoiceDataLens > 0 )
	{
		vcState = voiceCapture->GetVoiceData( rawCaptureData.GetData() + curRawCaptureData , maxRawCaptureDataSize - curRawCaptureData , newVoiceDataLens );
		curRawCaptureData = newVoiceDataLens + curRawCaptureData;
		if ( vcState != EVoiceCaptureState::Ok )
		{
			UKismetSystemLibrary::PrintString( this , TEXT( "Voice Buffer is too small" ) );
		}
		compressCaptureData();
	}
}

void UNetworkAvatarComponent::compressCaptureData()
{
	uint32 realCompressedSize = maxRawCaptureDataSize;
	int32 remainder = voiceEncoder->Encode( rawCaptureData.GetData() , curRawCaptureData , compressedData.GetData() , realCompressedSize );
	if ( remainder > 0 )
	{
		TArray<uint8> temp;
		temp.AddUninitialized( remainder );

		int32 copyStart = curRawCaptureData - remainder;
		FMemory::Memcpy( temp.GetData() , rawCaptureData.GetData() + copyStart , remainder );

		FMemory::Memcpy( rawCaptureData.GetData() , temp.GetData() , remainder );
	}
	curRawCaptureData = remainder;

	sendVoiceData( compressedData , realCompressedSize );
}

void UNetworkAvatarComponent::sendVoiceData( const TArray<uint8> &voiceData , int32 realSize )
{
	Avatar *avatar = static_cast<Avatar*>( networkEntity );
	avatar->sendVoiceDataToOther( voiceData , realSize );
}

void UNetworkAvatarComponent::onGetVoiceData( const TArray<uint8> &data )
{
	if ( networkEntity == nullptr )return;
	if ( isLocallyControl ) return;
	if ( !networkEntity->inWorld() ) return;

	if ( ownerPawn )
	{
		ownerPawn->onReceiveVoiceData( data );
	}
}

void UNetworkAvatarComponent::sendCharMoveSpeed()
{
	if ( !networkEntity->inWorld() ) return;

	if ( ownerPawn )
	{
		float curSpeed = ownerPawn->walkSpeed;
		if ( curSpeed != lastCharMoveSpeed )
		{
			Avatar *avatar = static_cast<Avatar*>( networkEntity );
			avatar->sendCharMoveSpeed( curSpeed );
		}
		lastCharMoveSpeed = curSpeed;
	}
}

void UNetworkAvatarComponent::OnGetCharMoveSpeed( float speed )
{
	if ( networkEntity == nullptr )return;
	if ( isLocallyControl ) return;
	if ( !networkEntity->inWorld() ) return;

	if ( ownerPawn )
	{
		ownerPawn->walkSpeed = speed;
	}
}

void UNetworkAvatarComponent::sendCustomCMD( const FString& cmd , const FString& paramStr , bool isNetworkSend )
{
	if ( !isNetworkSend ) return;
	if ( networkEntity == nullptr ) return;
	if ( !networkEntity->inWorld() ) return;

	Avatar *avatar = static_cast<Avatar*>( networkEntity );
	avatar->sendCustomCMD( cmd , paramStr );
}

void UNetworkAvatarComponent::onGetCustomCMD( const FString& cmd , const FString& paramStr )
{
	if ( networkEntity == nullptr )return;
	if ( !networkEntity->inWorld() ) return;
	if ( isLocallyControl ) return;
	if ( !IsAvatarGuide() ) return;

	//注意 此处接收到消息的对象 并不是本客户端的操作人 而是远端引导员在本客户端的复制
	//所以 需要首先GetPlayerPawn(0) 来获取本客户端的操作人
	if ( cmd.Equals( TEXT( "OpenLevel" ) ) )
	{
		AVRCharacterBase *pawn = Cast<AVRCharacterBase>( UGameplayStatics::GetPlayerPawn( this , 0 ) );
		pawn->networkPlayer->logoutSpace();

		UVRGameInstance *gameInstance = Cast<UVRGameInstance>( UGameplayStatics::GetGameInstance( this ) );
		gameInstance->isOpenLevel = true;

		UGameplayStatics::OpenLevel( this , FName( *paramStr ) );
	}
	else if ( cmd.Equals( TEXT( "LogoutGame" ) ) )
	{
		AVRCharacterBase *pawn = Cast<AVRCharacterBase>( UGameplayStatics::GetPlayerPawn( this , 0 ) );
		pawn->networkPlayer->logoutGame();

		UGameplayStatics::OpenLevel( this , FName( TEXT( "SelectRole" ) ) );
	}
}

bool UNetworkAvatarComponent::getSyncVoice()
{
	return isSyncVoice && !isPauseSyncVoice;
}

void UNetworkAvatarComponent::setSyncVoice( bool isMute )
{
	isSyncVoice = !isMute;
	
	if ( !voiceCapture.IsValid() )return;

	if ( isSyncVoice && getHasNetwork() && networkEntity->inWorld() )
	{
		if ( !voiceCapture->IsCapturing() )
		{
			voiceCapture->Start();
		}
	}
	else
	{
		if ( voiceCapture->IsCapturing() )
		{
			voiceCapture->Shutdown();
		}
	}
}
