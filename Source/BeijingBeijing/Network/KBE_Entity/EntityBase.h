#pragma once

#include "Entity.h"

class UNetworkBaseComponent;

class EntityBase : public Entity
{
public:
	EntityBase();
	virtual ~EntityBase();
	
	void linkToUnrealObject( UNetworkBaseComponent *networkObj );
	void breakFromUnrealObject();

protected:

	UNetworkBaseComponent * unrealObject;
};

#define ENTITYDEF_DECLARE_EntityBase(PARENT_MODULE)