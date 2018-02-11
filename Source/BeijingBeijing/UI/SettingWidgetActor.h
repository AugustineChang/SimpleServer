// Copyright 2017-2020 nasuVR All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SettingWidgetActor.generated.h"

UCLASS()
class BEIJINGBEIJING_API ASettingWidgetActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASettingWidgetActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void updateHandWidgetRotation( float deltaTime );
	void updateHandWidgetScale( float deltaTime );

	UFUNCTION()
	void OnClickWidgetBtn( UPrimitiveComponent* OverlappedComp , AActor* OtherActor , UPrimitiveComponent* OtherComp , int32 OtherBodyIndex , bool bFromSweep , const FHitResult& SweepResult );

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FVector getWidgetNormal();

protected:

	UPROPERTY( VisibleDefaultsOnly, Category = "SettingWidgetActor" )
	class UWidgetComponent *menuWidget;

	UPROPERTY( VisibleDefaultsOnly, Category = "SettingWidgetActor" )
	USceneComponent *originPoint;

	UPROPERTY( VisibleDefaultsOnly , Category = "SettingWidgetActor" )
	USceneComponent *WidgetPoint;

	UPROPERTY( VisibleDefaultsOnly , Category = "SettingWidgetActor" )
	class UStaticMeshComponent *clickMesh;

	UPROPERTY( EditAnywhere , Category = "SettingWidgetActor" )
	float widgetRadius;

	UPROPERTY( EditAnywhere , Category = "SettingWidgetActor" )
	USoundWave *clickSound;

	FVector widgetToCamera;
	bool isShowWidget;
	bool isPlayScaleWidgetAnim;
	float savedScale;
	float delayTimer;
};
