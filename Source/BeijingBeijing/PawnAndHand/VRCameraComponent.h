// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "Camera/CameraComponent.h"
#include "VRCameraComponent.generated.h"

/**
 * 
 */
UCLASS( meta = ( BlueprintSpawnableComponent ) )
class BEIJINGBEIJING_API UVRCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

	UVRCameraComponent( const FObjectInitializer& ObjectInitializer );

public:
	/**
	* Returns camera's Point of View.
	* Called by Camera class. Subclass and postprocess to add any effects.
	*/
	UFUNCTION(BlueprintCallable, Category = Camera)
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraSettings )
	bool IsUpdateRotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraSettings )
	bool IsUpdatePosition;

	FVector GetHMDInputLocation();
	FVector GetDeltaLocation();

private:

	FQuat VROrientation;
	FVector VRPosition;

	FQuat LastVROrientation;
	FVector LastVRPosition;
};
