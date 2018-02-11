// Copyright 2017-2020 nasuVR All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DataTables.generated.h"

UCLASS( Blueprintable , BlueprintType )
class BEIJINGBEIJING_API UTableRow : public UObject
{
	GENERATED_BODY()

public:
	
	UFUNCTION( BlueprintCallable , BlueprintPure , Category = "DataTable" )
	bool getRowData( const FString &columnName , FString &outStr );

	UPROPERTY( BlueprintReadOnly , Category = "DataTable" )
	TMap<FString , FString> rowMap;

	UPROPERTY( BlueprintReadOnly , Category = "DataTable" )
	FString rowKey;
};

UCLASS( Blueprintable , BlueprintType )
class BEIJINGBEIJING_API UTable : public UObject
{
	GENERATED_BODY()

public:

	TArray<FString> getRowNames();
	bool getTableData( const FString &rowName , UTableRow *& outRow );

	UPROPERTY()
	TMap<FString , UTableRow*> tableMap;
};


UCLASS( Blueprintable , BlueprintType )
class BEIJINGBEIJING_API UScenicSpotList : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY()
	TArray<FString> spotList;
};