#pragma once
#include "KBECommon.h"

class EntityBase;

class GeneralObj : public EntityBase
{
public:
	GeneralObj();
	virtual ~GeneralObj();

public:
	virtual void __init__() override;
	virtual void onDestroy() override;

	void pickupObject( int32 avatarID , bool isLeftHand );
	void dropObject( int32 avatarID , bool isLeftHand );

	void onPickupObj( int32 avatarID , uint8 isLeftHand );
	void onDropObj( int32 avatarID , uint8 isLeftHand );

public:
	FString objTypeName;
};

#define ENTITYDEF_DECLARE_GeneralObj(PARENT_MODULE)\
	ENTITYDEF_PROPERTY_REGISTER( PARENT_MODULE , objTypeName )\
	ENTITYDEF_METHOD_ARGS2_REGISTER( PARENT_MODULE , onPickupObj )\
	ENTITYDEF_METHOD_ARGS2_REGISTER( PARENT_MODULE , onDropObj )
