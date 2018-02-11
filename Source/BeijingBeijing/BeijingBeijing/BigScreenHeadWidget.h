// Copyright 2017-2020 nasuVR All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BeijingBeijing/WeatherRequest.h"
#include "BigScreenHeadWidget.generated.h"

/**
 * 
 */
UCLASS()
class BEIJINGBEIJING_API UBigScreenHeadWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UBigScreenHeadWidget( const FObjectInitializer& ObjectInitializer );
	
public:

	UFUNCTION( BlueprintImplementableEvent , Category = "BeijingBeijing" )
	void updateDataTime( const FString &timeStr );

	UFUNCTION( BlueprintImplementableEvent , Category = "BeijingBeijing" )
	void updateWeather( const FWeatherData& weatherData );
};
