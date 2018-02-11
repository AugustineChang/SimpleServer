#pragma once
#include "KBECommon.h"

class EntityBase;

class Hand : public EntityBase
{
public:
	Hand();
	virtual ~Hand();

public:
	virtual void __init__() override;
	virtual void onDestroy() override;

	void sendHandLocation( const FVector &pos );
	void sendHandRotation( const FRotator &rot );

	void sendInputToOther( uint8 key , uint8 val );
	void sendAxisToOther( uint8 key , float val );

	void set_triggerState( uint8 trigger );
	void set_gripState( uint8 grip );
	void set_thumbstickAxis( float axis );

	void sendLaserStateToOther( uint8 laser );
	void onGetLaserState( uint8 laser );

	void sendGrabStateToOther( uint8 grab );
	void onGetGrabState( uint8 grab );

public:
	uint8 isLeft;
	int32 avatarEntityID;
	uint8 triggerState;
	uint8 gripState;
	float thumbstickAxis;
};

#define ENTITYDEF_DECLARE_Hand( PARENT_MODULE )\
	ENTITYDEF_PROPERTY_REGISTER( PARENT_MODULE , isLeft )\
	ENTITYDEF_PROPERTY_REGISTER( PARENT_MODULE , avatarEntityID )\
	ENTITYDEF_PROPERTY_WITH_SETMETHOD_REGISTER( PARENT_MODULE , triggerState )\
	ENTITYDEF_PROPERTY_WITH_SETMETHOD_REGISTER( PARENT_MODULE , gripState )\
	ENTITYDEF_PROPERTY_WITH_SETMETHOD_REGISTER( PARENT_MODULE , thumbstickAxis )\
	ENTITYDEF_METHOD_ARGS1_REGISTER( PARENT_MODULE , onGetLaserState )\
	ENTITYDEF_METHOD_ARGS1_REGISTER( PARENT_MODULE , onGetGrabState )
