// Copyright 2017-2020 nasuVR All Rights Reserved.

#include "LimitMotionController.h"
#include "Pickup/PickupCommonActor.h"
#include "components/BoxComponent.h"

ALimitMotionController::ALimitMotionController() : AMotionControllerBase()
{

}

bool ALimitMotionController::checkHandProximity()
{
	TArray<AActor*> actors;
	GrabBox->GetOverlappingActors( actors );

	for ( AActor *actor : actors )
	{
		//限制到 APickupCommonActor 及其 子类
		if ( !Cast<APickupCommonActor>( actor ) )continue;

		return true;
	}

	return false;
}

void ALimitMotionController::getNearestObject( AActor *&nearestActor )
{
	TArray<AActor*> actors;
	GrabBox->GetOverlappingActors( actors );

	nearestActor = nullptr;
	float nearestOverlap = 100000000.0f;
	FVector handLocation = GetActorTransform().TransformPosition( FVector( -10.0f , 0.0f , 0.0f ) );

	for ( AActor *actor : actors )
	{
		//限制到 APickupCommonActor 及其 子类
		if ( !Cast<APickupCommonActor>( actor ) )continue;

		FVector curLocation = actor->GetActorLocation();

		float curDist = FVector::DistSquared( handLocation , curLocation );
		if ( curDist < nearestOverlap )
		{
			nearestOverlap = curDist;
			nearestActor = actor;
		}
	}
}
