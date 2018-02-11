// Copyright 2017-2020 nasuVR All Rights Reserved.

#include "HandWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Layout/Anchors.h"
#include "Layout/Margin.h"


UHandWidget::UHandWidget( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer ) , startOffset( 76.0f , 38.5f ) , cellInterval( 160.0f, 85.0f ) ,
	rowNum( 3 ) , cellSize( 150.0f , 75.0f ) , cellAlign( 0.5f , 0.5f )
{

}

void UHandWidget::createChildWidgets()
{
	if ( !widgetRoot ) return;

	int32 len = childWidgetClass.Num();
	for ( int32 i = 0; i < len; ++i )
	{
		UUserWidget *newWidget = CreateWidgetOfClass( *childWidgetClass[i] , nullptr , GetWorld() , nullptr );
		UHandCellWidget *cellWidget = Cast<UHandCellWidget>( newWidget );
		
		UCanvasPanelSlot *canvasSlot = widgetRoot->AddChildToCanvas( cellWidget );
		canvasSlot->SetAnchors( FAnchors() );
		
		float xPos = startOffset.X + cellInterval.X * ( i % rowNum );
		float yPos = startOffset.Y + cellInterval.Y * ( i / rowNum );
		
		canvasSlot->SetPosition( FVector2D( xPos , yPos ) );
		canvasSlot->SetSize( cellSize );
		canvasSlot->SetAlignment( cellAlign );

		childWidget.Add( cellWidget );
	}
}

void UHandWidget::setCellWidgetShowHide( bool isShow )
{
	int32 len = childWidget.Num();
	for ( int32 i = 0; i < len; ++i )
	{
		childWidget[i]->SetCellShowHide( isShow );
	}
}

void UHandWidget::setCellWidgetScale( float scale )
{
	int32 len = childWidget.Num();
	for ( int32 i = 0; i < len; ++i )
	{
		childWidget[i]->SetCellScale( scale );
	}
}

void UHandWidget::NativeConstruct()
{
	Super::NativeConstruct();

	createChildWidgets();
}
