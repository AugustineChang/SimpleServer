// Copyright 2017-2020 nasuVR All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blueprint/UserWidget.h"
#include "BigScreenHeadWidget.h"
#include "BigScreenWidget.h"
#include "BigScreenCellWidget.h"
#include "BigScreenActor.generated.h"

class UWidgetComponent;
class UTableRow;

UCLASS()
class BEIJINGBEIJING_API ABigScreenActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABigScreenActor();

	void createWidgetByTable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay( const EEndPlayReason::Type EndPlayReason ) override;
	
	void updateScreenCellsAlpha( float deltaTime );
	void updateScreenTime();
	void updateWeatherData();

	UFUNCTION()
	void OnHoverScenicSpot( const FString &cmd , const FString &paramStr , bool isNetworkSend );
	UFUNCTION()
	void OnClickBigScreenCell( const FString &cmd , const FString &paramStr , bool isNetworkSend );

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	UPROPERTY( VisibleAnywhere , Category = "BeijingBeijing" )
	USceneComponent * root;

	UPROPERTY( VisibleAnywhere , Category = "BeijingBeijing" )
	UWidgetComponent * bigScreenHead;

	UPROPERTY( VisibleAnywhere , Category = "BeijingBeijing" )
	UWidgetComponent * bigScreenBody;

	UPROPERTY( EditAnywhere , Category = "BeijingBeijing" )
	TSubclassOf<UBigScreenHeadWidget> headClass;

	UPROPERTY( EditAnywhere , Category = "BeijingBeijing" )
	TSubclassOf<UBigScreenWidget> bodyClass;

	UPROPERTY( EditAnywhere , Category = "BeijingBeijing" )
	TSubclassOf<UBigScreenCellWidget> cellClass;

	UPROPERTY( EditAnywhere , Category = "BeijingBeijing" )
	FVector2D distance;

	UPROPERTY( EditAnywhere , Category = "BeijingBeijing" )
	FVector startOffset;

	UPROPERTY( EditAnywhere , Category = "BeijingBeijing" )
	int32 rowNum;

	UPROPERTY( EditAnywhere , Category = "BeijingBeijing" )
	FVector2D cellDrawSize;

	UPROPERTY( BlueprintReadOnly , Category = "Network" )
	TArray<UWidgetComponent*> widgetsList;

	FTimerHandle timerHandle;
	int32 widgetsNum;

	bool isHoverSpot;
	bool isPlayCellsAlpha;
	float cellTimer;
	bool isShowCell;

	bool isSelectCell;
};
