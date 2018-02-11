// Copyright 2017-2020 nasuVR All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HandInterface.generated.h"

// This class does not need to be modified.
UINTERFACE( BlueprintType )
class UHandInterface : public UInterface
{
	GENERATED_BODY()
};

class AMotionControllerBase;

/**
 * 
 */
class BEIJINGBEIJING_API IHandInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	UFUNCTION( BlueprintNativeEvent , BlueprintCallable , Category = "LaserInterface" )
	void OnPickup( AMotionControllerBase *hand );

	UFUNCTION( BlueprintNativeEvent , BlueprintCallable , Category = "LaserInterface" )
	void OnDrop( AMotionControllerBase *hand );

	UFUNCTION( BlueprintNativeEvent , BlueprintCallable , Category = "LaserInterface" )
	void OnProximity( AMotionControllerBase *hand );

	UFUNCTION( BlueprintNativeEvent , BlueprintCallable , Category = "LaserInterface" )
	void OnLeave( AMotionControllerBase *hand );
};
