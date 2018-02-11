// Copyright 2017-2020 nasuVR All Rights Reserved.

#include "HandCellWidget.h"


UHandCellWidget::UHandCellWidget( const FObjectInitializer& ObjectInitializer ) 
	: Super( ObjectInitializer ) , isShowCell( false ) , isPlayScaleAnime( false )
{
}

void UHandCellWidget::SetCellShowHide( bool isShow )
{
	isShowCell = isShow;
	isPlayScaleAnime = true;
	SetVisibility( ESlateVisibility::SelfHitTestInvisible );
}

void UHandCellWidget::SetCellScale( float scale )
{
	SetRenderScale( FVector2D( scale , scale ) );
	bool hasCollision = scale >= 1.0f;
	SetVisibility( hasCollision ? ESlateVisibility::Visible : ESlateVisibility::SelfHitTestInvisible );
}

void UHandCellWidget::NativeTick( const FGeometry& MyGeometry , float InDeltaTime )
{
	Super::NativeTick( MyGeometry , InDeltaTime );

	updateCellScale( InDeltaTime );
}

void UHandCellWidget::updateCellScale( float deltaTime )
{
	if ( !isPlayScaleAnime ) return;

	float targetScale = isShowCell ? 1.0f : 0.0f;
	float curScale = RenderTransform.Scale.X;
	float lerpScale = FMath::Lerp( curScale , targetScale , deltaTime * 5.0f );
	SetRenderScale( FVector2D( lerpScale , lerpScale ) );

	if ( FMath::IsNearlyEqual( lerpScale , targetScale , 0.001f ) )
	{
		isPlayScaleAnime = false;
		SetVisibility( ESlateVisibility::Visible );
	}
}
