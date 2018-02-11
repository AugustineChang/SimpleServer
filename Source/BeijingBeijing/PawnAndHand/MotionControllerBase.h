// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Other/LaserActor.h"
#include "UI/SettingWidgetActor.h"
#include "MotionControllerBase.generated.h"

UENUM(Blueprintable)
enum class HandState :uint8
{
	open = 0,
	canGrab = 1,
	grab = 2
};

UCLASS()
class BEIJINGBEIJING_API AMotionControllerBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMotionControllerBase();

	UFUNCTION( BlueprintCallable , BlueprintPure , Category = "MotionControl" )
	ALaserActor *getLaserActor();

	void updatePlayerIndex( int32 playerIndex );
	void rumbleController( float intensity );

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual bool checkHandProximity();
	virtual void getNearestObject( AActor *&nearestActor );
	void handCheckInterface();
	void setHandState( HandState newState );
	void spawnHandWidget();
	void updateTouchWidgetRotation( float deltaTime );

	void initActionBind();
	
	UFUNCTION()
	void OnTriggerPressed();
	
	UFUNCTION()
	void OnTriggerReleased();

	UFUNCTION()
	void OnGripPressed();

	UFUNCTION()
	void OnGripReleased();

	UFUNCTION()
	void OnThumbStickPressed();

	UFUNCTION()
	void OnThumbStickReleased();

	UFUNCTION()
	void ThumbstickTouch(float value);

	UFUNCTION()
	void OnTouchWidget( UPrimitiveComponent* OverlappedComp , AActor* OtherActor , UPrimitiveComponent* OtherComp , int32 OtherBodyIndex , bool bFromSweep , const FHitResult& SweepResult );

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	UPROPERTY( EditAnywhere , Category = "MotionControl" )
	EControllerHand chirality;

	UPROPERTY( EditAnywhere , Category = "MotionControl" )
	TSubclassOf<ALaserActor> laserClass;

	UPROPERTY( EditAnywhere, Category = "MotionControl")
	TSubclassOf<ASettingWidgetActor> settingWidgetActorClass;

	UPROPERTY( EditAnywhere , Category = "MotionControl" )
	class UHapticFeedbackEffect_Base *hapticEffect;

	UPROPERTY()
	class ASettingWidgetActor *settingWidgetActor;

public:

	UPROPERTY(BlueprintReadWrite, Category = "MotionControl")
	HandState handState;

	UPROPERTY( VisibleDefaultsOnly , Category = "MotionControl" )
	class UNetworkHandComponent *networkHandComp;

	UPROPERTY(BlueprintReadWrite, Category = "MotionControl")
	float middleFingerValue;

	UPROPERTY(BlueprintReadWrite, Category = "MotionControl")
	float indexFingerValue;

	UPROPERTY(BlueprintReadWrite, Category = "MotionControl")
	float thumbFingerValue;

protected:

	UPROPERTY( VisibleDefaultsOnly , Category = "MotionControl" )
	class USkeletalMeshComponent *handMesh;

	UPROPERTY( VisibleDefaultsOnly , Category = "MotionControl" )
	class UVRControllerComponent *ControllerComponent;

	UPROPERTY( VisibleDefaultsOnly , Category = "MotionControl" )
	class UBoxComponent *GrabBox;

	UPROPERTY( VisibleDefaultsOnly , Category = "MotionControl" )
	class USphereComponent *TouchSphere;

	UPROPERTY( VisibleDefaultsOnly , Category = "MotionControl" )
	class UWidgetInteractionComponent *touchWidgetComp;

	UPROPERTY( VisibleDefaultsOnly , Category = "MotionControl" )
	class UChildActorComponent *laserChildComp;

	AActor *lastNearestObj;
	AActor *grabedObject;
	ALaserActor *laserActor;

	bool lastHandProximity;
	bool ifPressedThumbStick;

private:	
	float currentTimeToShow;
	float currentTimeToHide;
};