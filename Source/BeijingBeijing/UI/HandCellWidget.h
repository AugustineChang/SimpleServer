// Copyright 2017-2020 nasuVR All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HandCellWidget.generated.h"

/**
 * 
 */
UCLASS()
class BEIJINGBEIJING_API UHandCellWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UHandCellWidget( const FObjectInitializer& ObjectInitializer );

	UFUNCTION( BlueprintCallable , Category = "HandWidget" )
	void SetCellShowHide( bool isShow );

	UFUNCTION( BlueprintCallable , Category = "HandWidget" )
	void SetCellScale( float scale );

protected:

	virtual void NativeTick( const FGeometry& MyGeometry , float InDeltaTime ) override;
	
	void updateCellScale( float deltaTime );

protected:
	
	bool isShowCell;
	bool isPlayScaleAnime;
};
