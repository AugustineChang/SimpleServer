// Copyright@2017

#include "PhySoundComponent.h"
#include "Other/BP_Util.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"


//TMap <UPhySoundComponent::Pair, USoundCue* > UPhySoundComponent::soundList;
static TMap<EPhysicalSurface, bool> initSoundSurfaceType()
{
	TMap<EPhysicalSurface, bool> temp;
	temp.Add(EPhysicalSurface::SurfaceType1, true);//wood
	temp.Add(EPhysicalSurface::SurfaceType2, true);//Glass
	temp.Add(EPhysicalSurface::SurfaceType3, true);//Rock
	temp.Add(EPhysicalSurface::SurfaceType4, false);//grass
	temp.Add(EPhysicalSurface::SurfaceType5, false);//Shortgrass
	temp.Add(EPhysicalSurface::SurfaceType6, false);//Carpet
	temp.Add(EPhysicalSurface::SurfaceType7, false);//cloth
	temp.Add(EPhysicalSurface::SurfaceType8, true);//concrete
	temp.Add(EPhysicalSurface::SurfaceType9, false);//paper
	temp.Add(EPhysicalSurface::SurfaceType10, true);//metal
	temp.Add(EPhysicalSurface::SurfaceType11, true);//plastic
	temp.Add(EPhysicalSurface::SurfaceType12, false);//rubber
	temp.Add(EPhysicalSurface::SurfaceType13, false);//leather
	return temp;
}


static TMap<EPhysicalSurface, bool> soundList = initSoundSurfaceType();

UPhySoundComponent::UPhySoundComponent( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer ) , canPlay( false ) , selfMat( nullptr ) ,
	velocityThreshold( 100.0f ) , RestitutionFactor( 0.6f ) , debug( false ) ,
	cueVolume( 1.0f ) , rangeMax( 500 ) , interval( 0.05f ) , timer( 0.0f )
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UPhySoundComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor	*parent = GetOwner();
	UActorComponent* temp = parent->GetComponentByClass(UStaticMeshComponent::StaticClass());
	if (!temp)
	{
		UE_LOG(LogTemp, Error, TEXT("no StaticMesh"));
		return;
	}
	staticComp = Cast<UStaticMeshComponent>( temp );
	staticComp->OnComponentHit.AddDynamic(this, &UPhySoundComponent::OnHit);
	selfMat = staticComp->BodyInstance.GetSimplePhysicalMaterial();
	mass = staticComp->IsSimulatingPhysics() ? staticComp->GetMass() : 1.0f;

	LoadPhysicsAudio();
}


// Called every frame
void UPhySoundComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);   
	
	timer += DeltaTime;
	if (timer >= interval)
	{
		canPlay = true;		
	}
	else
	{
		canPlay = false;
	}
}

void UPhySoundComponent::LoadPhysicsAudio()
{
	//LoadSoundCue
	FString rootPath = "/Game/Sound/PhysMaterialSound/";
	const FString &surfaceTypeName = PhysicsTypeToString(selfMat);

	if (audioCueSoft == nullptr)
	{
		FString softPath = rootPath;
		softPath.Append(surfaceTypeName).Append("_Soft");

		FStringAssetReference temRef(softPath);
		USoundCue *temp = UBP_Util::LoadObjFromPath<USoundCue>( temRef );
		if (temp == nullptr)
		{
			FStringAssetReference temRef2(TEXT("/Game/Sound/PhysMaterialSound/Default_Soft"));
			USoundCue *temp2 = UBP_Util::LoadObjFromPath<USoundCue>( temRef2 );
			audioCueSoft = temp2;
		}
		else
		{
			audioCueSoft = temp;
		}
	}

	if (audioCueHard == nullptr)
	{
		FString hardPath = rootPath;
		hardPath.Append(surfaceTypeName).Append("_Hard");

		FStringAssetReference temRef(hardPath);
		USoundCue *temp = UBP_Util::LoadObjFromPath<USoundCue>( temRef );
		if (temp == nullptr)
		{
			FStringAssetReference temRef2(TEXT("/Game/Sound/PhysMaterialSound/Default_Hard"));
			USoundCue *temp2 = UBP_Util::LoadObjFromPath<USoundCue>( temRef2 );
			audioCueHard = temp2;
		}
		else
		{
			audioCueHard = temp;
		}
	}
}

void UPhySoundComponent::OnHit(UPrimitiveComponent* overlapComp, AActor* otherActor, UPrimitiveComponent* otherComp,
	FVector temp, const FHitResult & sweepResult)
{
	if ( !canPlay )return;

	if ( !otherActor || ( otherActor == GetOwner() ) ) return;	

	const UPhysicalMaterial *otherMat = sweepResult.PhysMaterial.Get();

	if ( !selfMat || !otherMat ) return;

	threshold = velocityThreshold * mass;

	FBodyInstance *selfBodyInst = overlapComp->GetBodyInstance();
	FBodyInstance *otherBodyInst = otherComp->GetBodyInstance();

	FVector selfVel = selfBodyInst->GetUnrealWorldVelocity();//GetUnrealWorldVelocityAtPoint_AssumesLocked(sweepResult.Location);
	FVector otherVel = otherBodyInst->GetUnrealWorldVelocity();//GetUnrealWorldVelocityAtPoint_AssumesLocked(sweepResult.Location);

	FVector self2Hit = sweepResult.ImpactPoint - staticComp->GetComponentLocation();
	self2Hit.Normalize();
	FVector other2Hit = sweepResult.ImpactPoint - overlapComp->GetComponentLocation();
	other2Hit.Normalize();

	float dot1 = FVector::DotProduct( selfVel , self2Hit );
	float dot2 = FVector::DotProduct( otherVel , other2Hit );
	float impactVelMag = FMath::Abs( dot1 + dot2 );
	
	//if ( debug )
		//UKismetSystemLibrary::PrintString( this , FString::Printf( TEXT( "vel = %f + %f , %f" ) , dot1 , dot2 , threshold ) );
	
	if ( impactVelMag < threshold ) return;

	//¸Ä±äÒôÁ¿		
	if ( impactVelMag >= rangeMax )
	{
		cueVolume = 1.0f;
	}
	else if ( impactVelMag >= threshold )
	{
		cueVolume = ( impactVelMag - threshold ) / ( rangeMax - threshold );
	}

	if ( debug )
		UKismetSystemLibrary::PrintString( this , TEXT( "cue:" ) + FString::SanitizeFloat( cueVolume ) + TEXT( "     " ) + TEXT( "vel:" ) + FString::SanitizeFloat( impactVelMag ) );

	EPhysicalSurface selfType = selfMat->SurfaceType.GetValue();
	EPhysicalSurface otherType = otherMat->SurfaceType.GetValue();
	bool *selfPtr = soundList.Find(selfType);
	bool *otherPtr = soundList.Find(otherType);

	if (!selfPtr || !otherPtr)
	{
		if (audioCueHard)
		{
			UGameplayStatics::SpawnSoundAttached(audioCueHard, staticComp, NAME_None, FVector(ForceInit), EAttachLocation::KeepRelativeOffset, false, cueVolume);
			timer = 0.0f;
			canPlay = false;
		}
	}
	else
	{
		if (*otherPtr && *selfPtr)
		{
			if (audioCueHard)
			{
				UGameplayStatics::SpawnSoundAttached(audioCueHard, staticComp, NAME_None, FVector(ForceInit), EAttachLocation::KeepRelativeOffset, false, cueVolume);
				timer = 0.0f;
				canPlay = false;
			}
		}
		else
		{
			if (audioCueSoft)
			{
				UGameplayStatics::SpawnSoundAttached(audioCueSoft, staticComp, NAME_None, FVector(ForceInit), EAttachLocation::KeepRelativeOffset, false, cueVolume);
				timer = 0.0f;
				canPlay = false;
			}
		}
	}
}



FString UPhySoundComponent::PhysicsTypeToString(const UPhysicalMaterial *phsMat)
{
	switch (phsMat->SurfaceType.GetValue())
	{
	case SurfaceType1://Wood
		return FString("Wood");
	case SurfaceType2://Glass
		return FString("Glass");
	case SurfaceType3://Rock
		return FString("Rock");
	case SurfaceType4://Grass
		return FString("Grass");
	case SurfaceType5://Shortgrass
		return FString("Shortgrass");
	case SurfaceType6://Carpet
		return FString("Carpet");
	case SurfaceType7://Cloth
		return FString("Cloth");
	case SurfaceType8://Concrete
		return FString("Concrete");
	case SurfaceType9://Paper
		return FString("Paper");
	case SurfaceType10://Metal
		return FString("Metal");
	case SurfaceType11://Plastic
		return FString( "Plastic" );
	case SurfaceType12://Rubber
		return FString("Rubber");
	case SurfaceType13://Rubber
		return FString("Leather");
	default:
		return FString("Default");
	}
}

