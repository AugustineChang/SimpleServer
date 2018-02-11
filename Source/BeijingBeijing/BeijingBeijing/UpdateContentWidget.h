// Copyright 2017-2020 nasuVR All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UpdateContentWidget.generated.h"

/**
 * 
 */
UCLASS()
class BEIJINGBEIJING_API UUpdateContentWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	UUpdateContentWidget( const FObjectInitializer& ObjectInitializer );

	UFUNCTION( BlueprintImplementableEvent , Category = "BeijingBeijing" )
	void updateContent( const FString &rowKey , const FString &title , const FString &description );

	UFUNCTION( BlueprintImplementableEvent , Category = "BeijingBeijing" )
	void clearContent();

protected:

	UFUNCTION( BlueprintCallable , Category = "BeijingBeijing" )
	void OnClickOk();

	UFUNCTION( BlueprintCallable , Category = "BeijingBeijing" )
	void OnClickCancel();
};
