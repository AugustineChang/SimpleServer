#include "EntityBase.h"
#include "Network/NetworkComponent//NetworkBaseComponent.h"

EntityBase::EntityBase() : Entity() , unrealObject( nullptr )
{

}

EntityBase::~EntityBase()
{

}

void EntityBase::linkToUnrealObject( UNetworkBaseComponent *networkObj )
{
	unrealObject = networkObj;
}

void EntityBase::breakFromUnrealObject()
{
	unrealObject = nullptr;
}

