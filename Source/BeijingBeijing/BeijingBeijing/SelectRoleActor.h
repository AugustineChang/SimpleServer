// Copyright 2017-2020 nasuVR All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "interface/LaserInterface.h"
#include "SelectRoleActor.generated.h"

UCLASS()
class BEIJINGBEIJING_API ASelectRoleActor : public AActor , public ILaserInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASelectRoleActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void updateSpotLightIntensity( float deltaTime );

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION( BlueprintNativeEvent , BlueprintCallable , Category = "LaserInterface" )
	void OnHover();
	virtual void OnHover_Implementation() override;

	UFUNCTION( BlueprintNativeEvent , BlueprintCallable , Category = "LaserInterface" )
	void OnUnhover();
	virtual void OnUnhover_Implementation() override;

	UFUNCTION( BlueprintNativeEvent , BlueprintCallable , Category = "LaserInterface" )
	void OnPress( AMotionControllerBase *hand );
	virtual void OnPress_Implementation( AMotionControllerBase *hand ) override;

	UFUNCTION( BlueprintNativeEvent , BlueprintCallable , Category = "LaserInterface" )
	void OnRelease( AMotionControllerBase *hand );
	virtual void OnRelease_Implementation( AMotionControllerBase *hand ) override;

protected:

	UPROPERTY( VisibleAnywhere , Category = "BeijingBeijing" )
	USceneComponent * root;

	UPROPERTY( VisibleAnywhere , Category = "BeijingBeijing" )
	class UCapsuleComponent * CollisionCapsule;

	UPROPERTY( VisibleAnywhere , Category = "BeijingBeijing" )
	class USkeletalMeshComponent * bodyMesh;

	UPROPERTY( VisibleAnywhere , Category = "BeijingBeijing" )
	class USpotLightComponent *spotLight;
	
	UPROPERTY( VisibleAnywhere , Category = "BeijingBeijing" )
	UStaticMeshComponent *spotConeMesh;

	bool isHover;
	bool isPlayIntensityAnim;
	float curConeBrightness;
	class UMaterialInstanceDynamic *spotConeDynMat;
};
