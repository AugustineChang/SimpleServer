// Copyright 2017-2020 nasuVR All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Robot.generated.h"

UCLASS()
class BEIJINGBEIJING_API ARobot : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARobot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	// Called every frame
	virtual void Tick(float DeltaTime) override;	

public:

	UFUNCTION( BlueprintCallable , Category = "Robot" )
	bool CheckIfCloseToTarget();
	
	UFUNCTION( BlueprintCallable , Category = "Robot" )
	void playSoundByKey( const FString &tableName , const FString &key );

	UFUNCTION( BlueprintCallable , Category = "Robot" )
	void playSoundByName( const FString &fileName );

protected:

	void LookAtPlayer();

	UFUNCTION()
	void OnHoverScenicSpot( const FString &cmd , const FString &paramStr , bool isNetworkSend );
	UFUNCTION()
	void OnClickBigScreenCell( const FString &cmd , const FString &paramStr , bool isNetworkSend );
	UFUNCTION()
	void OnPlayVoice( const FString &cmd , const FString &paramStr , bool isNetworkSend );

protected:

	UPROPERTY( VisibleDefaultsOnly , Category = "Robot" )
	class UAudioComponent * audioPlayer;

	UPROPERTY(BlueprintReadWrite, Category = "Robot")
	bool bIsMovingToTar;

	UPROPERTY(BlueprintReadWrite, Category = "Robot")
	float maxWaitDistance;

	FString soundRoot;
	UPROPERTY()
	TMap<FString , USoundWave*> soundMap;
	bool isSelectCell;
};
