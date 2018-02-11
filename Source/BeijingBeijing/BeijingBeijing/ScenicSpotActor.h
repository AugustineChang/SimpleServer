// Copyright 2017-2020 nasuVR All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BeijingBeijing/WeatherRequest.h"
#include "Interface/LaserInterface.h"
#include "ScenicSpotActor.generated.h"

UCLASS()
class BEIJINGBEIJING_API AScenicSpotActor : public AActor , public ILaserInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AScenicSpotActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void initSpotActorByTable();
	void initSpotActorWeather();
	void updateSpotScale( float deltaTime );
	void updateBillboardUI( float deltaTime );

	UFUNCTION()
	void OnGetCustomAction( const FString &cmd , const FString &paramStr );

	UFUNCTION( BlueprintImplementableEvent , Category = "BeijingBeijing" )
	void OnGetTableData( const FString &name );

	UFUNCTION( BlueprintImplementableEvent , Category = "BeijingBeijing" )
	void OnGetWeatherData( const FWeatherData &weather );

	void hoverAction();
	void unhoverAction();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION( BlueprintNativeEvent , BlueprintCallable , Category = "LaserInterface" )
	void OnHover();
	virtual void OnHover_Implementation() override;

	UFUNCTION( BlueprintNativeEvent , BlueprintCallable , Category = "LaserInterface" )
	void OnUnhover();
	virtual void OnUnhover_Implementation() override;

	UFUNCTION( BlueprintNativeEvent , BlueprintCallable , Category = "LaserInterface" )
	void OnPress( AMotionControllerBase *hand );
	virtual void OnPress_Implementation( AMotionControllerBase *hand ) override;

	UFUNCTION( BlueprintNativeEvent , BlueprintCallable , Category = "LaserInterface" )
	void OnRelease( AMotionControllerBase *hand );
	virtual void OnRelease_Implementation( AMotionControllerBase *hand ) override;

protected:

	UPROPERTY( VisibleDefaultsOnly , Category = "BeijingBeijing" )
	USceneComponent *root;

	UPROPERTY( VisibleDefaultsOnly , Category = "BeijingBeijing" )
	class UBoxComponent *collisionBox;
	
	UPROPERTY( VisibleDefaultsOnly , Category = "BeijingBeijing" )
	USceneComponent *scalePoint;

	UPROPERTY( VisibleDefaultsOnly , Category = "BeijingBeijing" )
	class UStaticMeshComponent *spotMesh;

	UPROPERTY( VisibleDefaultsOnly , Category = "BeijingBeijing" )
	class UWidgetComponent *spotUI;

	UPROPERTY( EditAnywhere , Category = "BeijingBeijing" )
	EWeatherCityEnum weatherSpot;

	UPROPERTY( EditAnywhere , Category = "BeijingBeijing" )
	float hoverScale;

	UPROPERTY( EditAnywhere , Category = "BeijingBeijing" )
	float scaleAnimeSpeed;

	UPROPERTY( EditAnywhere , BlueprintReadOnly , Category = "BeijingBeijing" )
	FString spotKey;

	UPROPERTY( EditAnywhere , Category = "BeijingBeijing" )
	class USoundWave *hoverSound;

	UPROPERTY( EditAnywhere , Category = "BeijingBeijing" )
	class USoundWave *clickSound;

	bool isHover;
	bool isStartHoverAnime;
	bool isLocallyControl;
	FString levelName;
};
