// Copyright 2017-2020 nasuVR All Rights Reserved.

#include "UpdateContentWidget.h"
#include "GameControl/GlobalMessageDispatcher.h"

UUpdateContentWidget::UUpdateContentWidget( const FObjectInitializer& ObjectInitializer )
	:Super( ObjectInitializer )
{

}

void UUpdateContentWidget::OnClickOk()
{

}

void UUpdateContentWidget::OnClickCancel()
{
	bool hasMessager;
	UGlobalMessageDispatcher *messager = UGlobalMessageDispatcher::GetGlobalMessager( hasMessager );
	if ( hasMessager )
	{
		messager->DoCustomAction.Broadcast( TEXT( "BigScreenClick" ) , TEXT( "NoKey" ) , true );
	}
}
