#include "Avatar.h"
#include "EntityBase.h"
#include "KBEngine.h"
#include "LogicEvent.h"
#include "Network/NetworkComponent/NetworkAvatarComponent.h"

ENTITYDEF_CLASS_REGISTER( Avatar , EntityBase )

Avatar::Avatar() : EntityBase() , playerIndex(-1) , pingValue( -1 )
{
}

Avatar::~Avatar()
{
}

void Avatar::__init__()
{
}

void Avatar::onDestroy()
{
}

void Avatar::loginToSpace( uint8 mapKey )
{
	baseCall( "loginToSpace" , mapKey );
}

void Avatar::logoutSpace()
{
	baseCall( "logoutSpace" , (uint8)0 );
}

void Avatar::logoutGame()
{
	baseCall( "logoutGame" );
}

void Avatar::sendPingCall( int32 pingIndex )
{
	baseCall( "pingCall" , pingIndex );
}

void Avatar::echoPingCall( int32 pingIndex )
{
	if ( !unrealObject || !unrealObject->IsValidLowLevel() ) return;
	UNetworkAvatarComponent *avatarComp = Cast<UNetworkAvatarComponent>( unrealObject );
	avatarComp->recvPingCall( pingIndex );
}

void Avatar::sendPingValue( int32 ping )
{
	this->pingValue = ping;
	cellCall( "uploadPingValue" , ping );
}

void Avatar::sendCameraPosToOther( const FVector &pos )
{
	FVector kbPos;
	UE4Pos2KBPos( kbPos , pos );

	cellCall( "cameraPosSync" , kbPos );
}

void Avatar::sendCameraRotToOther( const FRotator &rot )
{
	FVector kbRot;
	UE4Dir2KBDir( kbRot , rot );

	cellCall( "cameraRotSync" , kbRot );
}

void Avatar::set_cameraPos( const FVector &pos )
{
	if ( !unrealObject || !unrealObject->IsValidLowLevel() ) return;
	UNetworkAvatarComponent *avatarComp = Cast<UNetworkAvatarComponent>( unrealObject );
	FVector uePos;
	KBPos2UE4Pos( uePos , this->cameraPos );
	avatarComp->OnGetCameraPos( uePos );
}

void Avatar::set_cameraRot( const FVector &rot )
{
	if ( !unrealObject || !unrealObject->IsValidLowLevel() ) return;
	UNetworkAvatarComponent *avatarComp = Cast<UNetworkAvatarComponent>( unrealObject );
	FRotator ueRot;
	KBDir2UE4Dir( ueRot , this->cameraRot );
	avatarComp->OnGetCameraRot( ueRot );
}

void Avatar::sendCharMoveSpeed( float speed )
{
	cellCall( "characterMoveSync" , speed );
}

void Avatar::set_characterMoveSpeed( float speed )
{
	if ( !unrealObject || !unrealObject->IsValidLowLevel() ) return;
	UNetworkAvatarComponent *avatarComp = Cast<UNetworkAvatarComponent>( unrealObject );
	avatarComp->OnGetCharMoveSpeed( this->characterMoveSpeed );
}

void Avatar::sendLipDataToOther( const TArray<float> &lipData )
{
	KB_ARRAY sendArray;
	sendArray.Append( lipData );

	cellCall( "lipDataSync" , sendArray );
}

void Avatar::onGetLipData( const KB_ARRAY &lipData )
{
	if ( !unrealObject || !unrealObject->IsValidLowLevel() ) return;
	UNetworkAvatarComponent *avatarComp = Cast<UNetworkAvatarComponent>( unrealObject );

	TArray<float> floatLipData;
	for ( const KBVar& val : lipData )
	{
		floatLipData.Add( val.GetValue<float>() );
	}

	avatarComp->onGetLipData( floatLipData );
}

void Avatar::sendVoiceDataToOther( const ByteArray &voiceData , int32 realSize )
{
	KB_ARRAY sendArray;
	for ( int32 i = 0; i < realSize; ++i )
	{
		sendArray.Add( voiceData[i] );
	}

	cellCall( "voiceChatSync" , sendArray );
}

void Avatar::onGetVoiceData( const KB_ARRAY &voiceData )
{
	if ( !unrealObject || !unrealObject->IsValidLowLevel() ) return;
	UNetworkAvatarComponent *avatarComp = Cast<UNetworkAvatarComponent>( unrealObject );

	TArray<uint8> uintVoiceData;
	for ( const KBVar& val : voiceData )
	{
		uintVoiceData.Add( val.GetValue<uint8>() );
	}

	avatarComp->onGetVoiceData( uintVoiceData );
}

void Avatar::sendCustomCMD( const FString &cmd , const FString &paramStr )
{
	FString sendCMD = cmd;
	sendCMD.Append( TEXT( ";" ) );
	sendCMD.Append( paramStr );

	cellCall( "doCustomAction" , sendCMD );
}

void Avatar::set_customCmd( const FString &cmd )
{
	bool hasMessager;
	UGlobalMessageDispatcher *messager = UGlobalMessageDispatcher::GetGlobalMessager( hasMessager );
	if ( hasMessager )
	{
		FString cmd_cmd;
		FString cmd_params;
		cmd.Split( TEXT( ";" ) , &cmd_cmd , &cmd_params );

		messager->OnGetCustomAction.Broadcast( cmd_cmd , cmd_params );
		messager->DoCustomAction.Broadcast( cmd_cmd , cmd_params , false );
	}
}
