// Copyright 2017-2020 nasuVR All Rights Reserved.

#include "LaserHittableActor.h"


// Sets default values
ALaserHittableActor::ALaserHittableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALaserHittableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALaserHittableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALaserHittableActor::OnHover_Implementation()
{

}

void ALaserHittableActor::OnUnhover_Implementation()
{

}

void ALaserHittableActor::OnPress_Implementation( AMotionControllerBase *hand )
{

}

void ALaserHittableActor::OnRelease_Implementation( AMotionControllerBase *hand )
{

}

