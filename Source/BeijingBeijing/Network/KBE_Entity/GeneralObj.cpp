#include "GeneralObj.h"
#include "EntityBase.h"
#include "KBEngine.h"
#include "Network/NetworkComponent/NetworkGeneralObjComponent.h"

ENTITYDEF_CLASS_REGISTER( GeneralObj , EntityBase )

GeneralObj::GeneralObj() : EntityBase()
{
}

GeneralObj::~GeneralObj()
{
}

void GeneralObj::__init__()
{
}

void GeneralObj::onDestroy()
{
}

void GeneralObj::pickupObject( int32 avatarID , bool isLeftHand )
{
	uint8 left = isLeftHand ? 1 : 0;
	cellCall( "pickupObj" , avatarID , left );
}

void GeneralObj::dropObject( int32 avatarID , bool isLeftHand )
{
	uint8 left = isLeftHand ? 1 : 0;
	cellCall( "dropObj" , avatarID , left );
}

void GeneralObj::onPickupObj( int32 avatarID , uint8 isLeftHand )
{
	if ( !unrealObject || !unrealObject->IsValidLowLevel() ) return;
	UNetworkGeneralObjComponent *objComp = Cast<UNetworkGeneralObjComponent>( unrealObject );
	objComp->onGetPickupEvent( avatarID , isLeftHand == 1 );
}

void GeneralObj::onDropObj( int32 avatarID , uint8 isLeftHand )
{
	if ( !unrealObject || !unrealObject->IsValidLowLevel() ) return;
	UNetworkGeneralObjComponent *objComp = Cast<UNetworkGeneralObjComponent>( unrealObject );
	objComp->onGetDropEvent( avatarID , isLeftHand == 1 );
}
