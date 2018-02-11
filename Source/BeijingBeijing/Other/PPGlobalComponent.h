// Copyright@2017

#pragma once

#include "Components/ActorComponent.h"
#include "PPGlobalComponent.generated.h"


UENUM(BlueprintType)
enum class EPPGlobalFlags : uint8
{
	HIGHLIGHT = 0 UMETA( DisplayName = "Hightlight" ) ,
	FLASH = 1 UMETA( DisplayName = "FlashTip" ) 
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BEIJINGBEIJING_API UPPGlobalComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPPGlobalComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	UFUNCTION( BlueprintCallable , Category = "PPGlobal" )
	void setPPFlag( EPPGlobalFlags flag , bool b = true );

protected:

	void config( int32 value );

	UPrimitiveComponent * overrideRenderingComponent;

	UPROPERTY(EditAnywhere, Meta = (BitMask,BitmaskEnum="EPPGlobalFlags"))
	int32 ppFlags;
	
};
