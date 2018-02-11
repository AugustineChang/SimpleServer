// Copyright 2017-2020 nasuVR All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Other/LaserActor.h"
#include "LimitLaserActor.generated.h"

/**
 * 
 */
UCLASS()
class BEIJINGBEIJING_API ALimitLaserActor : public ALaserActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ALimitLaserActor();

protected:

	virtual void laserTrace() override;
	
};
