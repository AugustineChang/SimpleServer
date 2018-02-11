// Copyright 2017-2020 nasuVR All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "MinimapUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class BEIJINGBEIJING_API UMinimapUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	UMinimapUserWidget( const FObjectInitializer& ObjectInitializer );

protected:
	// Called when the game starts or when spawned
	virtual void NativeConstruct() override;

	// Called every frame
	virtual void NativeTick( const FGeometry& MyGeometry , float InDeltaTime ) override;

public:

	UPROPERTY( EditAnywhere , BlueprintReadWrite , Category = "Minimap" )
		TArray<UTexture*> mapImages;
	UPROPERTY( BlueprintReadOnly , Category = "Minimap" )
		TArray<AActor*> charactersArray;
	UPROPERTY( EditAnywhere , BlueprintReadWrite , Category = "Minimap" )
		TArray<FVector2D> originPosArray;
	UPROPERTY( EditAnywhere , BlueprintReadWrite , Category = "Minimap" )
		TArray<FVector2D> mapSizeArray;
	UPROPERTY( EditAnywhere , BlueprintReadWrite , Category = "Minimap" )
		TArray<FVector2D> mapTexSizeArray;


	UPROPERTY( EditAnywhere , BlueprintReadWrite , Category = "Minimap" )
		float checkInterval;

	UPROPERTY( EditAnywhere , Category = "Minimap" )
		TSubclassOf<class AActor> characterClass;


public:

	UFUNCTION( BlueprintCallable , Category = "Minimap" )
		void toogleMap( bool b );

	UFUNCTION( BlueprintCallable , Category = "Minimap" )
		void changeMapIndex( int _mapIndex );


protected:

	UFUNCTION( BlueprintImplementableEvent , Category = "Minimap" )
		void setMapPointPos( int pointIndex , FVector2D pos );

	UFUNCTION( BlueprintImplementableEvent , Category = "Minimap" )
		void setMapImagePos( FVector2D pos );

	UFUNCTION( BlueprintImplementableEvent , Category = "Minimap" )
		void setMapImage( UTexture* mapImage , FVector2D mapSize , FVector2D mapTexSize );

	UFUNCTION( BlueprintImplementableEvent , Category = "Minimap" )
		void addMapPoints( int characterType );

	UFUNCTION( BlueprintImplementableEvent , Category = "Minimap" )
		void clearMapPoints();

private:

	bool bUpdatePos;
	FVector2D originPos;
	float timer;
	class AActor* selfCharacter;

	class ANetworkGameMode* networkGameMode;

private:

	void updatePointsPos();

	void updateCharactersArray();

	void refreshMapPoints();

	TArray<AActor*> getCurrentPlayers();
};
