// Copyright@2017

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "BP_Util.generated.h"

class UKBEventData;

UCLASS()
class BEIJINGBEIJING_API UBP_Util : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = Util, meta = (WorldContext = "WorldContextObject"))
	static int32 SetFlag(int32 flag, int32 oriFlag, bool enable);

	UFUNCTION( BlueprintCallable , Category = Util )
	static void fireNetworkEvent( const FString &eventName , UKBEventData *pEventData );

	UFUNCTION(BlueprintCallable, Category = Util , meta = (WorldContext = "WorldContextObject"))
	static void AdjustGlobalSoundVolume( const UObject *WorldContextObject , float InTransientMasterVolume);

	UFUNCTION(BlueprintCallable, Category = Util, meta = (WorldContext = "WorldContextObject"))
	static class USoundWave* GetSoundWaveFromFile(const FString& filePath);

	UFUNCTION( BlueprintCallable , BlueprintPure , Category = Util )
	static FString getCurTimeString( const FString &format );

	template <typename ObjClass>
	UFUNCTION( BlueprintCallable , Category = Load )
	static ObjClass* LoadObjFromPath( const FStringAssetReference &thePath )
	{
		if (!thePath.IsValid()) return nullptr;

		return Cast<ObjClass>(StaticLoadObject(ObjClass::StaticClass(), nullptr, *(thePath.ToString())));
	}

	UFUNCTION( BlueprintCallable , Category = Network )
	static void networkOpenLevel( class AVRCharacterBase *playerPawn , const FString &levelName , bool isBackToHall = false );

	UFUNCTION( BlueprintCallable , Category = Network )
	static void networkQuitGame( class AVRCharacterBase *playerPawn , const FString &offlineLevel );

private:

	static int fillSoundWaveInfo( USoundWave* sw , TArray<uint8>* rawFile );
};
