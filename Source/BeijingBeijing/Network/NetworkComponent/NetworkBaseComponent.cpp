// Copyright@2017

#include "NetworkBaseComponent.h"
#include "KBEngine.h"
#include "Network/KBE_Entity/EntityBase.h"

// Sets default values for this component's properties
UNetworkBaseComponent::UNetworkBaseComponent( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer ) , entityID( -1 ) , initLocallyControl( true )
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UNetworkBaseComponent::BeginPlay()
{
	isLocallyControl = initLocallyControl;

	Super::BeginPlay();
}

void UNetworkBaseComponent::EndPlay( EEndPlayReason::Type reason )
{
	KBENGINE_DEREGISTER_ALL_EVENT();
	Super::EndPlay( reason );
}

void UNetworkBaseComponent::OnComponentDestroyed( bool bDestroyingHierarchy )
{
	KBENGINE_DEREGISTER_ALL_EVENT();
	Super::OnComponentDestroyed( bDestroyingHierarchy );
}

void UNetworkBaseComponent::linkToKBEObject( EntityBase *entity , bool isLocal )
{
	this->networkEntity = entity;
	this->entityID = entity->id();
	this->isLocallyControl = isLocal;
}

void UNetworkBaseComponent::breakFromKBEObject()
{
	this->entityID = -1;
	this->isLocallyControl = true;
	this->networkEntity = nullptr;
}