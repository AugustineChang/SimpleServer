// Copyright 2017-2020 nasuVR All Rights Reserved.

#include "WeatherRequest.h"
#include "Dom/JsonValue.h"
#include "Dom/JsonObject.h"

UWeatherRequest* UWeatherRequest::instance = nullptr;

UWeatherRequest * UWeatherRequest::getSingleton()
{
	if ( instance == nullptr )
	{
		instance = NewObject<UWeatherRequest>();
		instance->AddToRoot();//prevent GC
	}
	return instance;
}

UWeatherRequest::UWeatherRequest()
{
	Http = &FHttpModule::Get();

	requestWeatherData( EWeatherCityEnum::beijing );
	requestWeatherData( EWeatherCityEnum::dongcheng );
	requestWeatherData( EWeatherCityEnum::xicheng );
	requestWeatherData( EWeatherCityEnum::haidian );
	requestWeatherData( EWeatherCityEnum::chaoyang );
}

bool UWeatherRequest::getWeatherData( EWeatherCityEnum city , int32 dayIndex , FWeatherData &outData )
{
	FString cityName = getCityName( city );
	USpotWeatherData **weatherData = weatherMap.Find( cityName );
	if ( !weatherData )
	{
		getRandomWeatherData( outData );
		return false;
	}

	if ( dayIndex < 0 || dayIndex >= 3 )
	{
		getRandomWeatherData( outData );
		return false;
	}

	outData = ( *weatherData )->dailyWeather[dayIndex];
	return true;
}

void UWeatherRequest::requestWeatherData( EWeatherCityEnum city )
{
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject( this , &UWeatherRequest::OnGetWeatherData );
	//This is the url on which to process the request

	FString url( TEXT( "https://free-api.heweather.com/s6/weather/forecast?key=b78f9dc8db87411b87c58296f1b28aef&location=" ) );
	url += getCityName( city );
	Request->SetURL( url );
	Request->SetVerb( "GET" );
	Request->SetHeader( TEXT( "User-Agent" ) , TEXT( "X-UnrealEngine-Agent" ) );
	Request->SetHeader( TEXT( "Content-Type" ) , TEXT( "application/json" ) );
	Request->ProcessRequest();
}

void UWeatherRequest::OnGetWeatherData( FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful )
{
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create( Response->GetContentAsString() );

	TSharedPtr<FJsonObject> JsonObject;
	if ( !FJsonSerializer::Deserialize( Reader , JsonObject ) ) return;

	TArray<TSharedPtr<FJsonValue>> rootArray = JsonObject->GetArrayField( TEXT( "HeWeather6" ) );
	TSharedPtr<FJsonObject> basicPart = rootArray[0]->AsObject()->GetObjectField( TEXT( "basic" ) );
	TArray<TSharedPtr<FJsonValue>> forecastPart = rootArray[0]->AsObject()->GetArrayField( TEXT( "daily_forecast" ) );
	if ( forecastPart.Num() <= 0 ) return;

	USpotWeatherData *spotData = NewObject<USpotWeatherData>();
	for ( int32 i = 0; i < forecastPart.Num(); ++i )
	{
		FWeatherData oneData;

		FString temp = forecastPart[i]->AsObject()->GetStringField( TEXT( "cond_code_d" ) );
		oneData.weatherID = FCString::Atoi( *temp );

		oneData.weatherName = forecastPart[i]->AsObject()->GetStringField( TEXT( "cond_txt_d" ) );
		oneData.dateStr = forecastPart[i]->AsObject()->GetStringField( TEXT( "date" ) );

		temp = forecastPart[i]->AsObject()->GetStringField( TEXT( "tmp_min" ) );
		oneData.temperatureMin = FCString::Atoi( *temp );

		temp = forecastPart[i]->AsObject()->GetStringField( TEXT( "tmp_max" ) );
		oneData.temperatureMax = FCString::Atoi( *temp );

		oneData.windDir = forecastPart[i]->AsObject()->GetStringField( TEXT( "wind_dir" ) );
		oneData.windSpeed = forecastPart[i]->AsObject()->GetStringField( TEXT( "wind_sc" ) );

		spotData->dailyWeather.Add( oneData );
	}

	weatherMap.Add( basicPart->GetStringField( TEXT( "cid" ) ) , spotData );
}

FString UWeatherRequest::getCityName( EWeatherCityEnum city )
{
	switch ( city )
	{
	case EWeatherCityEnum::beijing:
		return TEXT( "CN101010100" );

	case EWeatherCityEnum::dongcheng:
		return TEXT( "CN101011600" );

	case EWeatherCityEnum::xicheng:
		return TEXT( "CN101011700" );

	case EWeatherCityEnum::haidian:
		return TEXT( "CN101010200" );

	case EWeatherCityEnum::chaoyang:
		return TEXT( "CN101010300" );

	default:
		return TEXT( "Error" );
	}
}

void UWeatherRequest::getRandomWeatherData( FWeatherData &outData )
{
	outData.dateStr = UKismetMathLibrary::Now().ToString( TEXT( "%Y-%m-%d" ) );

	//temperature -10 ~ 30 随机
	int32 rand1 = UKismetMathLibrary::RandomInteger( 40 );
	int32 rand2 = UKismetMathLibrary::RandomInteger( rand1 );
	outData.temperatureMax = rand1 - 10;
	outData.temperatureMin = rand2 - 10;

	outData.weatherID = getWeatherID( UKismetMathLibrary::RandomInteger( 8 ) );
	outData.weatherName = getWeatherName( outData.weatherID );

	outData.windDir = getWindDir( UKismetMathLibrary::RandomInteger( 9 ) );
	outData.windSpeed = getWindSpeed( UKismetMathLibrary::RandomInteger( 5 ) );
}

FString UWeatherRequest::getWeatherName( int32 weatherID )
{
	switch ( weatherID )
	{
	case 100:
		return TEXT( "晴" );

	case 101:
		return TEXT( "多云" );

	case 104:
		return TEXT( "阴" );

	case 200:
		return TEXT( "有风" );

	case 300:
		return TEXT( "阵雨" );

	case 305:
		return TEXT( "小雨" );

	case 306:
		return TEXT( "中雨" );

	case 307:
		return TEXT( "大雨" );

	default:
		return TEXT( "Error" );
	}
}

int32 UWeatherRequest::getWeatherID( int32 index )
{
	switch ( index )
	{
	case 0:
		return 100;

	case 1:
		return 101;

	case 2:
		return 104;

	case 3:
		return 200;

	case 4:
		return 300;

	case 5:
		return 305;

	case 6:
		return 306;

	case 7:
		return 307;

	default:
		return 100;
	}
}

FString UWeatherRequest::getWindDir( int32 index )
{
	switch ( index )
	{
	case 0:
		return TEXT( "无风向" );

	case 1:
		return TEXT( "东风" );

	case 2:
		return TEXT( "西风" );

	case 3:
		return TEXT( "北风" );

	case 4:
		return TEXT( "南风" );

	case 5:
		return TEXT( "东南风" );

	case 6:
		return TEXT( "西北风" );

	case 7:
		return TEXT( "东北风" );

	case 8:
		return TEXT( "西南风" );

	default:
		return TEXT( "无风向" );
	}
}

FString UWeatherRequest::getWindSpeed( int32 index )
{
	switch ( index )
	{
	case 0:
		return TEXT( "微风" );

	case 1:
		return TEXT( "1-2" );

	case 2:
		return TEXT( "2-3" );

	case 3:
		return TEXT( "3-4" );

	case 4:
		return TEXT( "4-5" );

	default:
		return TEXT( "微风" );
	}
}
