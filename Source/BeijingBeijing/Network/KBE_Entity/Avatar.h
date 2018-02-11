#pragma once
#include "KBECommon.h"

class EntityBase;

class Avatar : public EntityBase
{
public:
	Avatar();
	virtual ~Avatar();

public:
	virtual void __init__() override;
	virtual void onDestroy() override;

	void loginToSpace( uint8 mapKey );
	void logoutSpace();
	void logoutGame();

	void sendPingCall( int32 pingIndex );
	void echoPingCall( int32 pingIndex );
	void sendPingValue( int32 pingValue );

	void sendCameraPosToOther( const FVector &position );
	void sendCameraRotToOther( const FRotator &rotation );
	void set_cameraPos( const FVector &position );
	void set_cameraRot( const FVector &rotation );

	void sendCharMoveSpeed( float speed );
	void set_characterMoveSpeed( float speed );

	void sendLipDataToOther( const TArray<float> &lipData );
	void onGetLipData( const KB_ARRAY &lipData );

	void sendVoiceDataToOther( const ByteArray &voiceData , int32 realSize );
	void onGetVoiceData( const KB_ARRAY &voiceData );

	void sendCustomCMD( const FString &cmd , const FString &paramStr );
	void set_customCmd( const FString &cmd );

public:
	FString name;
	uint8 authority;
	uint8 characterType;
	FString ipAddr;
	int32 pingValue;
	int32 playerIndex;

	FVector cameraPos;
	FVector cameraRot;
	float characterMoveSpeed;

	FString customCmd;
};

#define ENTITYDEF_DECLARE_Avatar(PARENT_MODULE)\
	ENTITYDEF_PROPERTY_REGISTER( PARENT_MODULE , name )\
	ENTITYDEF_PROPERTY_REGISTER( PARENT_MODULE , authority )\
	ENTITYDEF_PROPERTY_REGISTER( PARENT_MODULE , characterType )\
	ENTITYDEF_PROPERTY_REGISTER( PARENT_MODULE , ipAddr )\
	ENTITYDEF_PROPERTY_REGISTER( PARENT_MODULE , playerIndex )\
	ENTITYDEF_PROPERTY_REGISTER( PARENT_MODULE , pingValue )\
	ENTITYDEF_PROPERTY_WITH_SETMETHOD_REGISTER( PARENT_MODULE , cameraPos )\
	ENTITYDEF_PROPERTY_WITH_SETMETHOD_REGISTER( PARENT_MODULE , cameraRot )\
	ENTITYDEF_PROPERTY_WITH_SETMETHOD_REGISTER( PARENT_MODULE , characterMoveSpeed )\
	ENTITYDEF_PROPERTY_WITH_SETMETHOD_REGISTER( PARENT_MODULE , customCmd )\
	ENTITYDEF_METHOD_ARGS1_REGISTER( PARENT_MODULE , onGetLipData )\
	ENTITYDEF_METHOD_ARGS1_REGISTER( PARENT_MODULE , onGetVoiceData )\
	ENTITYDEF_METHOD_ARGS1_REGISTER( PARENT_MODULE , echoPingCall )

