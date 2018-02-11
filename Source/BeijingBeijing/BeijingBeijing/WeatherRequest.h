// Copyright 2017-2020 nasuVR All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Http.h"
#include "WeatherRequest.generated.h"

USTRUCT( BlueprintType )
struct FWeatherData
{
	GENERATED_BODY()

	UPROPERTY( BlueprintReadOnly , Category = "Weather" )
	int32 weatherID;

	UPROPERTY( BlueprintReadOnly , Category = "Weather" )
	FString weatherName;

	UPROPERTY( BlueprintReadOnly , Category = "Weather" )
	FString dateStr;

	UPROPERTY( BlueprintReadOnly , Category = "Weather" )
	int32 temperatureMin;

	UPROPERTY( BlueprintReadOnly , Category = "Weather" )
	int32 temperatureMax;

	UPROPERTY( BlueprintReadOnly , Category = "Weather" )
	FString windDir;

	UPROPERTY( BlueprintReadOnly , Category = "Weather" )
	FString windSpeed;
};

UENUM( BlueprintType )
enum class EWeatherCityEnum : uint8
{
	beijing 	 UMETA( DisplayName = "Beijing" ) ,
	dongcheng 	 UMETA( DisplayName = "DongCheng" ) ,
	haidian      UMETA( DisplayName = "HaiDian" ) ,
	xicheng	     UMETA( DisplayName = "XiCheng" ) ,
	chaoyang     UMETA( DisplayName = "ChaoYang" )
};

UCLASS( Blueprintable , BlueprintType )
class BEIJINGBEIJING_API USpotWeatherData : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY( BlueprintReadOnly , Category = "Weather" )
	TArray<FWeatherData> dailyWeather;
};

UCLASS()
class BEIJINGBEIJING_API UWeatherRequest : public UObject
{
	GENERATED_BODY()
	
public:
	
	static UWeatherRequest * getSingleton();

	// Sets default values for this actor's properties
	UWeatherRequest();
	
	UFUNCTION( BlueprintCallable , Category = "Weather" )
	bool getWeatherData( EWeatherCityEnum city , int32 dayIndex , FWeatherData &outData );

protected:

	void requestWeatherData( EWeatherCityEnum city );
	void OnGetWeatherData( FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful );

	FString getCityName( EWeatherCityEnum city );
	
	void getRandomWeatherData( FWeatherData &outData );
	FString getWeatherName( int32 weatherID );
	int32 getWeatherID( int32 index );
	FString getWindDir( int32 index );
	FString getWindSpeed( int32 index );

	static UWeatherRequest *instance;
	class FHttpModule * Http;

	UPROPERTY()
	TMap<FString , USpotWeatherData*> weatherMap;
};
