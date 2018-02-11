// Copyright@2017

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetworkBaseComponent.generated.h"

class EntityBase;

UCLASS( ClassGroup = ( Custom ) , meta = ( BlueprintSpawnableComponent ) )
class BEIJINGBEIJING_API UNetworkBaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UNetworkBaseComponent( const FObjectInitializer& ObjectInitializer );

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay( EEndPlayReason::Type reason ) override;
	virtual void OnComponentDestroyed( bool bDestroyingHierarchy ) override;

public:	

	virtual void linkToKBEObject( EntityBase * networkEntity , bool isLocal );
	virtual void breakFromKBEObject();

	inline int32 getEntityID()
	{
		return entityID;
	}

	inline bool getIsLocalControl()
	{
		return isLocallyControl;
	}

	UFUNCTION( BlueprintCallable , BlueprintPure , Category = Network )
	inline bool getHasNetwork()
	{
		return networkEntity != nullptr && entityID > 0;
	}

protected:

	UPROPERTY( EditDefaultsOnly , Category = Network )
	bool initLocallyControl;

	UPROPERTY( VisibleAnywhere , BlueprintReadOnly , Category = Network )
	int32 entityID;

	UPROPERTY( VisibleAnywhere , BlueprintReadOnly , Category = Network )
	bool isLocallyControl;

	EntityBase *networkEntity;
};
