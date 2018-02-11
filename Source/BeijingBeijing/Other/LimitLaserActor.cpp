// Copyright 2017-2020 nasuVR All Rights Reserved.

#include "LimitLaserActor.h"
#include "Pickup/LaserHittableActor.h"
#include "kismet/KismetSystemLibrary.h"

ALimitLaserActor::ALimitLaserActor() : ALaserActor()
{

}

void ALimitLaserActor::laserTrace()
{
	FVector start = laserRoot->GetComponentLocation();
	FVector end = start + laserRoot->GetForwardVector() * laserMaxDistance;

	isTraceHit = UKismetSystemLibrary::LineTraceSingle( this , start , end , TraceTypeQuery3 , false , ignoredActors , EDrawDebugTrace::None , hitResult , true );

	//限制必须打在ALaserHittableActor 及其 子类下
	if ( isTraceHit && !Cast<ALaserHittableActor>( hitResult.Actor.Get() ) )
	{
		isTraceHit = false;
	}

	if ( isTraceHit )
	{
		float realDist = ( hitResult.ImpactPoint - start ).Size();
		laserDistance = realDist;

		if ( !laserEndPoint->IsVisible() )
			laserEndPoint->SetVisibility( true );
	}
	else
	{
		if ( laserMode == LaserModeType::Always )
		{
			laserDistance = laserMaxDistance;
		}

		if ( laserEndPoint->IsVisible() )
			laserEndPoint->SetVisibility( false );
	}
}
