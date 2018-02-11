// Copyright@2017

#include "PPGlobalComponent.h"
#include "BP_Util.h"


// Sets default values for this component's properties
UPPGlobalComponent::UPPGlobalComponent()
{
}

// Called when the game starts
void UPPGlobalComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!overrideRenderingComponent)
	{
		overrideRenderingComponent = Cast<UPrimitiveComponent>(GetOwner()->GetComponentByClass(UPrimitiveComponent::StaticClass()));
		if (!overrideRenderingComponent) 
		{
			UE_LOG(LogTemp, Error, TEXT("no StaticMesh"));
		}
	}
	overrideRenderingComponent->SetRenderCustomDepth(true);
	config(ppFlags);
}

void UPPGlobalComponent::config(int32 value)
{
	overrideRenderingComponent->SetCustomDepthStencilValue(value);
}

void UPPGlobalComponent::setPPFlag( EPPGlobalFlags flag , bool b ) 
{
	ppFlags = UBP_Util::SetFlag( 1 << static_cast<int32>( flag ) , ppFlags , b );
	config( ppFlags );
}
