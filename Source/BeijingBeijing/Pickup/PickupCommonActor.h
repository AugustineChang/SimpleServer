// Copyright 2017-2020 nasuVR All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/HandInterface.h"
#include "PickupCommonActor.generated.h"

class AMotionControllerBase;

UCLASS()
class BEIJINGBEIJING_API APickupCommonActor : public AActor , public IHandInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupCommonActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void initOriginData();
	float GetPointProjectionOnSegment( const FVector &PointA , const FVector &PointB , const FVector &Point );
	FVector GetPointOnSegment( const FVector &PointA , const FVector &PointB , float Factor );
	void UpdateLocationAndRotation();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION( BlueprintNativeEvent , BlueprintCallable , Category = "LaserInterface" )
	void OnPickup( AMotionControllerBase *hand );
	virtual void OnPickup_Implementation( AMotionControllerBase *hand ) override;

	UFUNCTION( BlueprintNativeEvent , BlueprintCallable , Category = "LaserInterface" )
	void OnDrop( AMotionControllerBase *hand );
	virtual void OnDrop_Implementation( AMotionControllerBase *hand ) override;

	UFUNCTION( BlueprintNativeEvent , BlueprintCallable , Category = "LaserInterface" )
	void OnProximity( AMotionControllerBase *hand );
	virtual void OnProximity_Implementation( AMotionControllerBase *hand ) override;

	UFUNCTION( BlueprintNativeEvent , BlueprintCallable , Category = "LaserInterface" )
	void OnLeave( AMotionControllerBase *hand );
	virtual void OnLeave_Implementation( AMotionControllerBase *hand ) override;

protected:

	UPROPERTY( VisibleAnywhere , Category = "PickupCommonActor" )
	class UStaticMeshComponent * pickupMesh;
	
	UPROPERTY( VisibleAnywhere , Category = "PickupCommonActor" )
	class UPPGlobalComponent *ppGlobal;

	USceneComponent *firstHand;
	USceneComponent *secondHand;

	bool savedSimulatePhysics;
	ECollisionResponse initialResponse;
	FVector originVector;// secondHand -> firsthand
	float originLocRatio;//pickupMesh origin Location Ratio
	FVector originLocOffset;//pickupMesh origin Location Offset
	FRotator originRotation;//pickupMesh origin Rotation
};
