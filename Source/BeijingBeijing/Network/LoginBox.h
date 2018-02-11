// Copyright@2017

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LoginBox.generated.h"

class UNetworkAccountComponent;

UCLASS()
class BEIJINGBEIJING_API ALoginBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALoginBox();

	UFUNCTION( BlueprintNativeEvent , BlueprintCallable , Category = LoginBox )
	void OnLoginGame();

	UFUNCTION( BlueprintNativeEvent , BlueprintCallable , Category = LoginBox )
	void OnOfflineGame();

	UFUNCTION( BlueprintNativeEvent , BlueprintCallable , Category = LoginBox )
	void OnStartGame();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	
	UPROPERTY( VisibleAnywhere , Category = "Network" )
	USceneComponent * rootComp;

	UPROPERTY( VisibleAnywhere , Category = "Network" )
	UNetworkAccountComponent * networkComp;
};
