// Copyright 2017-2020 nasuVR All Rights Reserved.

#include "MinimapUserWidget.h"
#include "Engine/StaticMeshActor.h"
#include "Engine.h"
#include "Network/NetworkGameMode.h"
#include "Network/NetworkComponent/NetworkAvatarComponent.h"
#include "PawnAndHand/VRCharacterBase.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
UMinimapUserWidget::UMinimapUserWidget( const FObjectInitializer& ObjectInitializer ) : Super( ObjectInitializer ) ,checkInterval( 1.0f )
{

}

// Called when the game starts or when spawned
void UMinimapUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	timer = checkInterval;

	selfCharacter = UGameplayStatics::GetPlayerCharacter( this , 0 );

	networkGameMode = Cast<ANetworkGameMode>( UGameplayStatics::GetGameMode( this ) );
}

// Called every frame
void UMinimapUserWidget::NativeTick( const FGeometry& MyGeometry , float InDeltaTime )
{
	Super::NativeTick( MyGeometry , InDeltaTime );

	if( !bUpdatePos )return;
	timer -= InDeltaTime;
	if( timer <= 0 )
	{
		updateCharactersArray();
		timer = checkInterval;
	}
	updatePointsPos();
}

void UMinimapUserWidget::toogleMap( bool b )
{
	bUpdatePos = b;
	timer = checkInterval;
	updateCharactersArray();
}

void UMinimapUserWidget::changeMapIndex( int _mapIndex )
{
	if( originPosArray.IsValidIndex( _mapIndex ) )
	{
		originPos = originPosArray[ _mapIndex ];
	}

	if( mapImages.IsValidIndex( _mapIndex ) )
	{
		setMapImage( mapImages[ _mapIndex ] , mapSizeArray[ _mapIndex ] , mapTexSizeArray[_mapIndex] );
	}
}

TArray<AActor*> UMinimapUserWidget::getCurrentPlayers()
{
	if( networkGameMode == nullptr )
	{
		return charactersArray;
	}
	else
	{
		return networkGameMode->getAllPlayers();
	}
}

void UMinimapUserWidget::updatePointsPos()
{
	FVector selfPos = selfCharacter->GetActorLocation();

	setMapImagePos( FVector2D( originPos.X - selfPos.X , originPos.Y - selfPos.Y ) );

	for( int i = 0; i < charactersArray.Num(); i++ )
	{
		FVector tempPos = charactersArray[ i ]->GetActorLocation() - selfPos;

		setMapPointPos( i , FVector2D( tempPos.X , tempPos.Y ) );
	}
}

void UMinimapUserWidget::updateCharactersArray()
{
	if( !getCurrentPlayers().IsValidIndex( 0 ) )
	{
		// 单机
		if( charactersArray.Num() == 0 )
		{

			UGameplayStatics::GetAllActorsOfClass( this , characterClass , charactersArray );
			refreshMapPoints();
		}
	}
	else
	{
		if( getCurrentPlayers().Num() == charactersArray.Num() )return;

		charactersArray = getCurrentPlayers();

		refreshMapPoints();
	}
}

void UMinimapUserWidget::refreshMapPoints()
{
	clearMapPoints();
	for( int i = 0; i < charactersArray.Num(); i++ )
	{
		int tempIndex = 0;
		// 0-self,1-guide,2-other,如果0&1，取0
		if( charactersArray[ i ] == selfCharacter )
		{
			tempIndex = 0;
		}
		else
		{
			AVRCharacterBase* characterBase = Cast<AVRCharacterBase>( charactersArray[ i ] );
			if( characterBase )
			{
				tempIndex = characterBase->networkPlayer->IsAvatarGuide() ? 1 : 2;
			}
		}
		addMapPoints( tempIndex );
	}
}


