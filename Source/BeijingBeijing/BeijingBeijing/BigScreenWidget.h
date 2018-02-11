// Copyright 2017-2020 nasuVR All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BigScreenWidget.generated.h"

class ABigScreenActor;
class UUpdateContentWidget;
class UTableRow;

/**
 * 
 */
UCLASS()
class BEIJINGBEIJING_API UBigScreenWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UBigScreenWidget( const FObjectInitializer& ObjectInitializer );
	
public:

	void initWidgetActor( ABigScreenActor *bigScreen );

	void OnScenicSpotHoverState( bool isHover , UTableRow *table );
	void OnTourRouteState( bool isShowRoute , UTableRow *table );

protected:

	UFUNCTION( BlueprintCallable , Category = "BeijingBeijing" )
	void ConstructWidget();

	UFUNCTION( BlueprintCallable , Category = "BeijingBeijing" )
	void TickWidget( float DeltaTime );
	
	bool fadeInPanel( float deltaTime , UUserWidget *widget );
	bool waitPanel( float deltaTime , float wiatTime );
	bool fadeOutPanel( float deltaTime , UUserWidget *widget );

	UFUNCTION( BlueprintImplementableEvent , Category = "BeijingBeijing" )
	void OnShowPanel( UUserWidget *curWidget );
	
	ABigScreenActor *ownerActor;
	float waitTimer;

	UPROPERTY()
	UUserWidget *curWidget;
	UPROPERTY()
	UUserWidget *mainWidget;
	UPROPERTY()
	UUpdateContentWidget *scenicSpotWidget;
	UPROPERTY()
	UUpdateContentWidget *tourRouteWidget;

protected://BeginPlayչʾUI

	void showPanelsOnBegin();
	void updateShowPanels( float deltaTime );
	void createOnePanel();
	void OnShowPanelFinish();

	UPROPERTY( EditDefaultsOnly , BlueprintReadWrite , Category = "BeijingBeijing" )
	TArray<TSubclassOf<UUserWidget>> displayList;

	UPROPERTY( EditDefaultsOnly , BlueprintReadWrite , Category = "BeijingBeijing" )
	TArray<float> displayWaitTime;

	UPROPERTY( BlueprintReadOnly , Category = "BeijingBeijing" )
	float isPlayDisplayAnim;
	int32 displayStage;
	int32 panelIndex;

protected://SwapUI

	void startSwapWidget( UUserWidget *newTo );
	void updateSwapWidget( float deltaTime );
	void swapTwoWidget();

	UPROPERTY( EditDefaultsOnly , Category = "BeijingBeijing" )
	TSubclassOf<UUpdateContentWidget> scenicSpotWidgetClass;

	UPROPERTY( EditDefaultsOnly , Category = "BeijingBeijing" )
	TSubclassOf<UUpdateContentWidget> tourRouteWidgetClass;
	
	UUserWidget *toWidget;

	float isPlaySwapPanel;
	int32 swapStage;

protected:

	bool isShowTourRoute;
	bool isHoverSpot;
	UTableRow *savedSpotTable;
};
