// Copyright 2017-2020 nasuVR All Rights Reserved.

#include "BigScreenWidget.h"
#include "BeijingBeijing/UpdateContentWidget.h"
#include "BigScreenActor.h"
#include "Other/DataTables.h"

UBigScreenWidget::UBigScreenWidget( const FObjectInitializer& ObjectInitializer )
	:Super( ObjectInitializer ) , waitTimer( 0.0f ) , isPlayDisplayAnim( false ) , panelIndex( -1 ) ,
	isPlaySwapPanel( false ) , swapStage( 0 ) , displayStage( 0 ) , isShowTourRoute( false ) ,
	isHoverSpot( false )
{
}

void UBigScreenWidget::initWidgetActor( ABigScreenActor *bigScreen )
{
	ownerActor = bigScreen;
}

void UBigScreenWidget::OnScenicSpotHoverState( bool isHover , UTableRow *table )
{
	savedSpotTable = table;
	isHoverSpot = isHover;

	if ( isPlayDisplayAnim )return;
	if ( isShowTourRoute ) return;
	
	if ( isHoverSpot )
	{
		startSwapWidget( scenicSpotWidget );

		FString outName;
		FString outDesc;
		bool ret1 = table->getRowData( TEXT( "name" ) , outName );
		bool ret2 = table->getRowData( TEXT( "description" ) , outDesc );

		if ( ret1 && ret2 )
		{
			scenicSpotWidget->updateContent( table->rowKey , outName , outDesc );
		}
	}
	else
	{
		startSwapWidget( mainWidget );
	}
}

void UBigScreenWidget::OnTourRouteState( bool isShowRoute , UTableRow *table )
{
	isShowTourRoute = isShowRoute;

	if ( isShowTourRoute )
	{
		startSwapWidget( tourRouteWidget );

		FString outName;
		FString outDesc;
		bool ret1 = table->getRowData( TEXT( "name" ) , outName );
		bool ret2 = table->getRowData( TEXT( "description" ) , outDesc );

		if ( ret1 && ret2 )
		{
			tourRouteWidget->updateContent( table->rowKey , outName , outDesc );
		}
	}
	else
	{
		startSwapWidget( mainWidget );
	}
}

void UBigScreenWidget::ConstructWidget()
{
	showPanelsOnBegin();

	if ( scenicSpotWidgetClass.Get() != nullptr )
	{
		scenicSpotWidget = CreateWidget<UUpdateContentWidget>( GetWorld() , *scenicSpotWidgetClass );
	}
	
	if ( tourRouteWidgetClass.Get() != nullptr )
	{
		tourRouteWidget = CreateWidget<UUpdateContentWidget>( GetWorld() , *tourRouteWidgetClass );
	}
}

void UBigScreenWidget::TickWidget( float DeltaTime )
{
	updateShowPanels( DeltaTime );
	updateSwapWidget( DeltaTime );
}

void UBigScreenWidget::showPanelsOnBegin()
{
	isPlayDisplayAnim = displayList.Num() > 0;
	if ( isPlayDisplayAnim ) displayStage = 0;
	else OnShowPanelFinish();
}

void UBigScreenWidget::updateShowPanels( float deltaTime )
{
	if ( !isPlayDisplayAnim ) return;

	switch ( displayStage )
	{
	case 0://生成UI
		createOnePanel();
		break;

	case 1://fade in
		if ( fadeInPanel( deltaTime , curWidget ) )
		{
			if ( panelIndex >= displayList.Num() - 1 )
			{
				displayStage = 0;
				isPlayDisplayAnim = false;
				OnShowPanelFinish();
			}
			else
				displayStage = 2;
		}
		break;

	case 2://展示等待
		if ( waitPanel( deltaTime , displayWaitTime[panelIndex] ) )
		{
			displayStage = 3;
		}
		break;

	case 3://fade out
		if ( fadeOutPanel( deltaTime , curWidget ) )
		{
			displayStage = 0;
		}
		break;

	default:
		break;
	}
}

void UBigScreenWidget::createOnePanel()
{
	if ( curWidget != nullptr )
	{
		curWidget->RemoveFromParent();
	}

	++panelIndex;
	curWidget = CreateWidgetOfClass( displayList[panelIndex] , nullptr , GetWorld() , nullptr );
	curWidget->SetColorAndOpacity( FLinearColor( 1.0f , 1.0f , 1.0f , 0.0f ) );
	OnShowPanel( curWidget );
	displayStage = 1;
	waitTimer = 0.0f;
}

bool UBigScreenWidget::fadeInPanel( float deltaTime , UUserWidget *widget )
{
	if ( widget == nullptr ) return true;

	float alpha = widget->ColorAndOpacity.A;
	alpha = FMath::Lerp( alpha , 1.0f , deltaTime * 10.0f );

	widget->SetColorAndOpacity( FLinearColor( 1.0f , 1.0f , 1.0f , alpha ) );

	return FMath::IsNearlyEqual( alpha , 1.0f , 0.001f );
}

bool UBigScreenWidget::waitPanel( float deltaTime , float waitTime )
{
	waitTimer += deltaTime;
	return waitTimer >= waitTime;
}

bool UBigScreenWidget::fadeOutPanel( float deltaTime , UUserWidget *widget )
{
	if ( widget == nullptr ) return true;

	float alpha = widget->ColorAndOpacity.A;
	alpha = FMath::Lerp( alpha , 0.0f , deltaTime * 10.0f );

	widget->SetColorAndOpacity( FLinearColor( 1.0f , 1.0f , 1.0f , alpha ) );

	return FMath::IsNearlyEqual( alpha , 0.0f , 0.001f );
}

void UBigScreenWidget::OnShowPanelFinish()
{
	if ( ownerActor )
	{
		ownerActor->createWidgetByTable();
	}

	mainWidget = curWidget;
	
	if ( savedSpotTable != nullptr )
	{
		OnScenicSpotHoverState( isHoverSpot , savedSpotTable );
		savedSpotTable = nullptr;
	}
}

void UBigScreenWidget::startSwapWidget( UUserWidget *newTo )
{
	isPlaySwapPanel = true;
	swapStage = 0;
	toWidget = newTo;
	toWidget->SetColorAndOpacity( FLinearColor( 1.0f , 1.0f , 1.0f , 0.0f ) );
}

void UBigScreenWidget::updateSwapWidget( float deltaTime )
{
	if ( !isPlaySwapPanel ) return;

	switch ( swapStage )
	{
	case 0://fade out
		if ( fadeOutPanel( deltaTime , curWidget ) )
		{
			swapStage = 1;
		}
		break;

	case 1://交换UI
		swapTwoWidget();
		break;

	case 2://fade in
		if ( fadeInPanel( deltaTime , toWidget ) )
		{
			swapStage = 0;
			toWidget = nullptr;
			isPlaySwapPanel = false;
		}
		break;

	default:
		break;
	}
}

void UBigScreenWidget::swapTwoWidget()
{
	if ( curWidget != nullptr )
	{
		curWidget->RemoveFromParent();
		curWidget = nullptr;
	}

	if ( toWidget != nullptr )
	{
		OnShowPanel( toWidget );
		curWidget = toWidget;
	}
	swapStage = 2;
}

