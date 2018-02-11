// Copyright 2017-2020 nasuVR All Rights Reserved.

#include "BigScreenCellWidget.h"
#include "Other/DataTables.h"
#include "kismet/GameplayStatics.h"
#include "GameControl/GlobalMessageDispatcher.h"


UBigScreenCellWidget::UBigScreenCellWidget( const FObjectInitializer& ObjectInitializer )
	:Super( ObjectInitializer )
{

}

void UBigScreenCellWidget::createCell_Implementation( int32 index , const FString &rowKey )
{
	this->index = index;
	this->tableRowKey = rowKey;

	UVRGameInstance *gameInstance = Cast<UVRGameInstance>( UGameplayStatics::GetGameInstance( this ) );
	if ( gameInstance )
	{
		gameInstance->getTableData( TEXT( "BigScreenTable" ) , rowKey , this->tableData );
	}
}

void UBigScreenCellWidget::OnClickCell()
{
	bool hasMessager;
	UGlobalMessageDispatcher *messager = UGlobalMessageDispatcher::GetGlobalMessager( hasMessager );
	if ( hasMessager )
	{
		messager->DoCustomAction.Broadcast( TEXT( "BigScreenClick" ) , tableRowKey , true );
	}
}
