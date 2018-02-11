// Copyright 2017-2020 nasuVR All Rights Reserved.

#include "DataTables.h"

TArray<FString> UTable::getRowNames()
{
	TArray<FString> rowNames;
	tableMap.GetKeys( rowNames );
	return rowNames;
}

bool UTable::getTableData( const FString &rowName , UTableRow *& outRow )
{
	outRow = nullptr;

	UTableRow **row = tableMap.Find( rowName );
	if ( !row ) return false;

	outRow = *row;
	return true;
}

bool UTableRow::getRowData( const FString &columnName , FString &outStr )
{
	FString *valueStr = rowMap.Find( columnName );
	if ( !valueStr ) return false;

	outStr = *valueStr;
	return true;
}
