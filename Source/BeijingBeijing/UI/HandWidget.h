// Copyright 2017-2020 nasuVR All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HandCellWidget.h"
#include "HandWidget.generated.h"


UCLASS()
class BEIJINGBEIJING_API UHandWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UHandWidget( const FObjectInitializer& ObjectInitializer );
	
	void createChildWidgets();
	
	UFUNCTION( BlueprintCallable , Category = "HandWidget" )
	void setCellWidgetShowHide( bool isShow );

	UFUNCTION( BlueprintCallable , Category = "HandWidget" )
	void setCellWidgetScale( float scale );

protected:

	virtual void NativeConstruct() override;

	UPROPERTY( EditDefaultsOnly , Category = "HandWidget" )
	TArray<TSubclassOf<UHandCellWidget>> childWidgetClass;

	UPROPERTY()
	TArray<UHandCellWidget*> childWidget;

	UPROPERTY( BlueprintReadWrite , Category = "HandWidget" )
	class UCanvasPanel *widgetRoot;

	UPROPERTY( EditDefaultsOnly , Category = "HandWidget" )
	FVector2D startOffset;

	UPROPERTY( EditDefaultsOnly , Category = "HandWidget" )
	FVector2D cellInterval;

	UPROPERTY( EditDefaultsOnly , Category = "HandWidget" )
	int32 rowNum;

	UPROPERTY( EditDefaultsOnly , Category = "HandWidget" )
	FVector2D cellSize;

	UPROPERTY( EditDefaultsOnly , Category = "HandWidget" )
	FVector2D cellAlign;
};
