// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "LaserActor.generated.h"

UENUM( Blueprintable )
enum class LaserModeType :uint8
{
	Close = 0 ,
	Nomral = 1 ,
	Always = 2
};

class UWidgetInteractionComponent;

UCLASS()
class BEIJINGBEIJING_API ALaserActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALaserActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FVector getCameraLocation();
	virtual void laserTrace();
	void laserCheckInterface();
	void GenerateVertex();
	void UpdateLaserRotation();
	void UpdateLaserEndPoint();
	void CalcLaserFade( float deltatime );

	void initActionBind();
	UFUNCTION()
	void OnTriggerPressed();
	UFUNCTION()
	void OnTriggerReleased();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void updatePlayerIndex( int32 playerIndex , bool isLeft );

	UFUNCTION( BlueprintCallable , Category = "Laser" )
	void SetLaserMode( LaserModeType type );

	UFUNCTION( BlueprintCallable , BlueprintPure , Category = "Laser" )
	LaserModeType GetLaserMode() { return laserMode; }

	UFUNCTION( BlueprintCallable , Category = "Laser" )
	void SetLaserEnable( int32 index , bool isEnable );

	UFUNCTION( BlueprintCallable , BlueprintPure , Category = "Laser" )
	bool GetLaserEnable();

protected:

	UPROPERTY( VisibleAnywhere , Category = "Laser" )
	USceneComponent * laserRoot;

	UPROPERTY( VisibleAnywhere , Category = "Laser" )
	UProceduralMeshComponent * laserMesh;

	UPROPERTY( VisibleAnywhere , Category = "Laser" )
	UStaticMeshComponent *laserEndPoint;

	UPROPERTY( VisibleAnywhere , Category = "Laser" )
	UWidgetInteractionComponent *widgetHitComp;

	UPROPERTY( EditAnywhere , Category = "Laser" )
	float laserMaxDistance;

	UPROPERTY( EditAnywhere , Category = "Laser" )
	float laserWidth;

	UPROPERTY( EditAnywhere , Category = "Laser" )
	float distanceToFade;
	
	UPROPERTY( EditAnywhere , Category = "Laser" )
	class UMaterialInterface *laserMaterial;
	
	TArray<bool> laserEnable;
	class AMotionControllerBase *handActor;
	bool isLaserLeft;

	FVector EndPointOffset;
	bool endPointRotByNormal;

	UPROPERTY()
	class UMaterialInstanceDynamic *laserDynMat;
	UPROPERTY()
	class UMaterialInstanceDynamic *laserEndDynMat;

	TArray<FVector> vertices;
	TArray<int32> indices;
	TArray<FVector> normals;
	TArray<FVector2D> uvs;
	TArray<FColor> vertexColors;
	TArray<FProcMeshTangent> tangents;
	TArray<AActor*> ignoredActors;

	bool isLackOfData;//缺少数据 不能运行
	FVector laserToCameraDir;
	bool isTraceHit;
	FHitResult hitResult;
	LaserModeType laserMode;
	float laserDistance;
	float laserAlpha;
	AActor *lastHitActor;
	AActor *pressedActor;

	LaserModeType savedLaserMode;
};
