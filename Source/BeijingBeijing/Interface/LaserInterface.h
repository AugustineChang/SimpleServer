// Copyright 2017-2020 nasuVR All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LaserInterface.generated.h"

// This class does not need to be modified.
UINTERFACE( BlueprintType )
class ULaserInterface : public UInterface
{
	GENERATED_BODY()
};

class AMotionControllerBase;

/**
 * 
 */
class BEIJINGBEIJING_API ILaserInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	UFUNCTION( BlueprintNativeEvent , BlueprintCallable , Category = "LaserInterface" )
	void OnHover();

	UFUNCTION( BlueprintNativeEvent , BlueprintCallable , Category = "LaserInterface" )
	void OnUnhover();

	UFUNCTION( BlueprintNativeEvent , BlueprintCallable , Category = "LaserInterface" )
	void OnPress( AMotionControllerBase * hand );

	UFUNCTION( BlueprintNativeEvent , BlueprintCallable , Category = "LaserInterface" )
	void OnRelease( AMotionControllerBase * hand );
};
