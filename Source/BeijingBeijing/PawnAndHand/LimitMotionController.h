// Copyright 2017-2020 nasuVR All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MotionControllerBase.h"
#include "LimitMotionController.generated.h"

/**
 * 
 */
UCLASS()
class BEIJINGBEIJING_API ALimitMotionController : public AMotionControllerBase
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ALimitMotionController();

protected:
	
	virtual bool checkHandProximity() override;
	virtual void getNearestObject( AActor *&nearestActor ) override;
};
