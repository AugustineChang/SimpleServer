// Copyright 2017-2020 nasuVR All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SundialActor.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class BEIJINGBEIJING_API ASundialActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASundialActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void updateSundialEmissive( float deltaTime );
	void updatePointerEmissive( float deltaTime );
	void calcPointerRotation();
	void bindTriggerEventDynamically( bool isBind );

	UFUNCTION()
	void OnTriggerPressed();
	UFUNCTION()
	void OnTriggerReleased();

	UFUNCTION()
	void OnApproachSundial( UPrimitiveComponent* OverlappedComp , AActor* OtherActor , UPrimitiveComponent* OtherComp , int32 OtherBodyIndex , bool bFromSweep , const FHitResult& SweepResult );
	UFUNCTION()
	void OnLeaveSundial( UPrimitiveComponent* OverlappedComp , AActor* OtherActor , UPrimitiveComponent* OtherComp , int32 OtherBodyIndex );

	UFUNCTION()
	void OnApproachPointer( UPrimitiveComponent* OverlappedComp , AActor* OtherActor , UPrimitiveComponent* OtherComp , int32 OtherBodyIndex , bool bFromSweep , const FHitResult& SweepResult );
	UFUNCTION()
	void OnLeavePointer( UPrimitiveComponent* OverlappedComp , AActor* OtherActor , UPrimitiveComponent* OtherComp , int32 OtherBodyIndex );

protected:
	
	UPROPERTY( VisibleAnywhere , Category = "BeijingBeijing" )
	USceneComponent * root;

	UPROPERTY( VisibleAnywhere , Category = "BeijingBeijing" )
	UStaticMeshComponent *sundialMesh;

	UPROPERTY( VisibleAnywhere , Category = "BeijingBeijing" )
	UBoxComponent *sundialBox;

	UPROPERTY( VisibleAnywhere , Category = "BeijingBeijing" )
	USceneComponent *sundialCenter;

	UPROPERTY( VisibleAnywhere , Category = "BeijingBeijing" )
	USceneComponent *sundialRotatePoint;

	UPROPERTY( VisibleAnywhere , Category = "BeijingBeijing" )
	UStaticMeshComponent *pointerMesh;

	UPROPERTY( VisibleAnywhere , Category = "BeijingBeijing" )
	UBoxComponent *pointerBox;

	bool isNearSundial;
	bool isPlaySundialEmissive;
	UMaterialInstanceDynamic *sundialDynMat;
	float curSundialEmissive;

	bool isNearPointer;
	bool isPlayPointerEmissive;
	UMaterialInstanceDynamic *pointerDynMat;
	float curPointerEmissive;

	AActor *sundialHandActor;
	AActor *pointerHandActor;
	bool isBindLeftHand;
	bool isTriggerPressed;

	bool savedNearSundial;
	bool savedNearPointer;
};
