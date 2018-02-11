#pragma once
#include "KBECommon.h"
#include "LogicEvent.h"
#include "EntityBase.h"

class Account : public EntityBase
{
public:
	Account();
	virtual ~Account();

public:
	virtual void __init__() override;
	virtual void onDestroy() override;
	
	void reqStartGame( const FString &name , uint8 authority , uint8 charType );
	void reqLogout();
	void onGameStarted( uint8 retCode );
};

#define ENTITYDEF_DECLARE_Account(PARENT_MODULE)\
	ENTITYDEF_METHOD_ARGS1_REGISTER( PARENT_MODULE , onGameStarted )
