#pragma once
#include "KBEvent.h"
#include "KBECommon.h"
#include "LogicEvent.generated.h"

UCLASS( Blueprintable , BlueprintType )
class BEIJINGBEIJING_API UKBEventDataCustom : public UKBEventData
{
	GENERATED_BODY()

public:

	FString cmd;
	FString paramStr;
};