// Copyright@2017

#pragma once

#include "CoreMinimal.h"
#include "NetworkBaseComponent.h"
#include "NetworkEntityComponent.generated.h"

class UKBEventData;

UCLASS( ClassGroup = ( Custom ) , meta = ( BlueprintSpawnableComponent ) )
class BEIJINGBEIJING_API UNetworkEntityComponent : public UNetworkBaseComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UNetworkEntityComponent( const FObjectInitializer& ObjectInitializer );

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	void lerpTransform( int32 index , float DeltaTime );

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void setupEntity( USceneComponent * targetComp , bool isRelative );
	void setUpdatePause( int32 index , bool isPausePos , bool isPauseRot );

	void checkSendTrans( int32 index , FVector &position , FRotator &rotation , bool &isPosValid , bool &isRotValid );

	void onGetPosition( int32 index , const FVector &position );
	void onGetRotation( int32 index , const FRotator &rotation );

	inline bool getIsSyncRelative( int32 index )
	{
		return isSyncRelative[index];
	}

protected:
	
	UPROPERTY( VisibleAnywhere , Category = Network )
	float lerpFactor;

	UPROPERTY( VisibleAnywhere , Category = Network )
	float slerpFactor;

	int32 componentsNum;
	TArray<USceneComponent*> targetSceneComponent;
	TArray<bool> isSyncRelative;

	TArray<bool> isPausePosUpdate;
	TArray<bool> isPauseRotUpdate;

	TArray<FVector> targetLocation;
	TArray<FQuat> targetRotation;
	TArray<bool> isReachTarget;

	TArray<FVector> lastSendLocation;
	TArray<FRotator> lastSendRotation;
	float intervalTime;
	TArray<float> lastSendTimer;
};
