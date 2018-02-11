// Copyright@2017

#pragma once

#include "Sound/SoundCue.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/PhysicsSettings.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "PhySoundComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BEIJINGBEIJING_API UPhySoundComponent : public UActorComponent
{
	GENERATED_BODY()

	static TArray<FString> s_predef_types;
	static bool init;
	static struct Pair 
	{
		UPhysicalMaterial *a;
		UPhysicalMaterial *b;
		Pair(UPhysicalMaterial* _a, UPhysicalMaterial* _b) : a(_a), b(_b) {}

		inline friend bool operator==( const Pair &lhs , const Pair &rhs )
		{
			bool result1 = ( lhs.a == rhs.a ) && ( lhs.b == rhs.b );
			bool result2 = ( lhs.a == rhs.b ) && ( lhs.b == rhs.a );
			return result1 || result2;
		}

		inline friend bool operator!=( const Pair &lhs , const Pair &rhs )
		{
			return !( lhs == rhs );
		}

		inline friend uint32 GetTypeHash( const Pair &pair )
		{
			uint32 hash_a = FCrc::MemCrc32( pair.a , sizeof(UPhysicalMaterial) );
			uint32 hash_b = FCrc::MemCrc32( pair.b , sizeof(UPhysicalMaterial) );
			return hash_a + hash_b;
		}
	};

public:

	UPhySoundComponent(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
	USoundCue *audioCueSoft;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
	USoundCue *audioCueHard;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
	float interval;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
	float velocityThreshold;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
	float RestitutionFactor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
	float rangeMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
	bool debug;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	UFUNCTION()
	void OnHit(class UPrimitiveComponent* overlapComp, class AActor* otherActor, class UPrimitiveComponent* otherComp,
			FVector temp, const FHitResult & sweepResult);

	void LoadPhysicsAudio();
	FString PhysicsTypeToString(const UPhysicalMaterial *phsMat);
	
	const UPhysicalMaterial *selfMat;
	UStaticMeshComponent *staticComp;

	bool canPlay;

	float timer;
	float mass;
	float threshold;
	float cueVolume;
};
