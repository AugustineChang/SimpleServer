// Copyright@2017

#include "NetworkActorInterface.h"

// This function does not need to be modified.
UNetworkActorInterface::UNetworkActorInterface(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

// Add default functionality here for any IHandEventHandler functions that are not pure virtual.
void INetworkActorInterface::OnJoinSpace() {};