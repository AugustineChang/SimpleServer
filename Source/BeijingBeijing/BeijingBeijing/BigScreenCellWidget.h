// Copyright 2017-2020 nasuVR All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BigScreenCellWidget.generated.h"

class UTableRow;

/**
 * 
 */
UCLASS()
class BEIJINGBEIJING_API UBigScreenCellWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UBigScreenCellWidget( const FObjectInitializer& ObjectInitializer );
	
public:

	UFUNCTION( BlueprintNativeEvent , Category = "BeijingBeijing" )
	void createCell( int32 index , const FString &rowKey );
	void createCell_Implementation( int32 index , const FString &rowKey );

	UFUNCTION( BlueprintImplementableEvent , Category = "BeijingBeijing" )
	void setAlpha( float alpha );

protected:

	UFUNCTION( BlueprintCallable , Category = "BeijingBeijing" )
	void OnClickCell();

	UPROPERTY( BlueprintReadOnly , Category = "BeijingBeijing" )
	int32 index;

	UPROPERTY( BlueprintReadOnly , Category = "BeijingBeijing" )
	FString tableRowKey;

	UPROPERTY( BlueprintReadOnly , Category = "BeijingBeijing" )
	UTableRow *tableData;
};
