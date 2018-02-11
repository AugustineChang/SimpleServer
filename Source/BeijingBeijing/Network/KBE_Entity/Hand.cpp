#include "Hand.h"
#include "EntityBase.h"
#include "KBEngine.h"
#include "Network/NetworkComponent/NetworkHandComponent.h"

ENTITYDEF_CLASS_REGISTER( Hand , EntityBase )

Hand::Hand() : EntityBase()
{
}

Hand::~Hand()
{
}

void Hand::__init__()
{
}

void Hand::onDestroy()
{
}

void Hand::sendHandLocation( const FVector &pos )
{
	FVector kbPos;
	UE4Pos2KBPos( kbPos , pos );

	cellCall( "updateHandLocation" , kbPos );
}

void Hand::sendHandRotation( const FRotator &rot )
{
	FVector kbRot;
	UE4Dir2KBDir( kbRot , rot );

	cellCall( "updateHandDirection" , kbRot );
}

void Hand::sendInputToOther( uint8 key , uint8 val )
{
	cellCall( "handInputSync" , key , val );
}

void Hand::sendAxisToOther( uint8 key , float val )
{
	cellCall( "handAxisSync" , key , val );
}

void Hand::set_triggerState( uint8 trigger )
{
	if ( !unrealObject || !unrealObject->IsValidLowLevel() ) return;
	UNetworkHandComponent *handComp = Cast<UNetworkHandComponent>( unrealObject );
	handComp->onGetInputAction( 0 , this->triggerState );
}

void Hand::set_gripState( uint8 grip )
{
	if ( !unrealObject || !unrealObject->IsValidLowLevel() ) return;
	UNetworkHandComponent *handComp = Cast<UNetworkHandComponent>( unrealObject );
	handComp->onGetInputAction( 1 , this->gripState );
}

void Hand::set_thumbstickAxis( float axis )
{
	if ( !unrealObject || !unrealObject->IsValidLowLevel() ) return;
	UNetworkHandComponent *handComp = Cast<UNetworkHandComponent>( unrealObject );
	handComp->onGetInputAxis( 2 , this->thumbstickAxis );
}

void Hand::sendLaserStateToOther( uint8 laser )
{
	cellCall( "handLaserStateSync" , laser );
}

void Hand::onGetLaserState( uint8 laser )
{
	if ( !unrealObject || !unrealObject->IsValidLowLevel() ) return;
	UNetworkHandComponent *handComp = Cast<UNetworkHandComponent>( unrealObject );
	handComp->onGetLaserState( laser );
}

void Hand::sendGrabStateToOther( uint8 grab )
{
	cellCall( "handGrabStateSync" , grab );
}

void Hand::onGetGrabState( uint8 grab )
{
	if ( !unrealObject || !unrealObject->IsValidLowLevel() ) return;
	UNetworkHandComponent *handComp = Cast<UNetworkHandComponent>( unrealObject );
	handComp->onGetGrabState( grab );
}
