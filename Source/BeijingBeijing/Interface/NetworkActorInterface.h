// Copyright@2017

#pragma once

#include "NetworkActorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE( meta = ( CannotImplementInterfaceInBlueprint ) , MinimalAPI )
class UNetworkActorInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
*
*/
class BEIJINGBEIJING_API INetworkActorInterface
{
	GENERATED_IINTERFACE_BODY()

		// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable)
	virtual void OnJoinSpace();
};
