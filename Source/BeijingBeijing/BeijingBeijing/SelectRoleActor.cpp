// Copyright 2017-2020 nasuVR All Rights Reserved.

#include "SelectRoleActor.h"
#include "components/CapsuleComponent.h"
#include "components/SpotLightComponent.h"
#include "components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"


// Sets default values
ASelectRoleActor::ASelectRoleActor() : isHover( false ) , isPlayIntensityAnim( false ) ,
curConeBrightness( 1.0f )
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	root = CreateDefaultSubobject<USceneComponent>( TEXT( "Root" ) );
	RootComponent = root;

	CollisionCapsule = CreateDefaultSubobject<UCapsuleComponent>( TEXT( "CollisionCapsule" ) );
	CollisionCapsule->SetupAttachment( RootComponent );
	CollisionCapsule->SetRelativeLocation( FVector::UpVector * 80.0f );
	CollisionCapsule->InitCapsuleSize( 30.0f , 88.0f );

	bodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>( TEXT( "BodyMesh" ) );
	bodyMesh->SetupAttachment( RootComponent );

	spotConeMesh = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "SpotConeMesh" ) );
	spotConeMesh->SetupAttachment( RootComponent );
	spotConeMesh->SetRelativeLocation( FVector::UpVector * 345.0f );
	spotConeMesh->SetRelativeRotation( FQuat::MakeFromEuler( FVector::ForwardVector * 180.0f ) );

	spotLight = CreateDefaultSubobject<USpotLightComponent>( TEXT( "SpotLight" ) );
	spotLight->SetupAttachment( RootComponent );
	spotLight->SetRelativeLocation( FVector::UpVector * 370.0f );
	spotLight->SetRelativeRotation( FQuat::MakeFromEuler( FVector::RightVector * -90.0f ) );
	spotLight->SetInnerConeAngle( 10.0f );
	spotLight->SetOuterConeAngle( 20.0f );
	spotLight->SetLightColor( FLinearColor( 0.4f , 0.6f , 1.0f ) );
	spotLight->SetIntensity( 1000.0f );
}

// Called when the game starts or when spawned
void ASelectRoleActor::BeginPlay()
{
	Super::BeginPlay();
	
	spotConeDynMat = spotConeMesh->CreateDynamicMaterialInstance( 0 );
}

void ASelectRoleActor::updateSpotLightIntensity( float deltaTime )
{
	if ( !isPlayIntensityAnim ) return;

	float targetIntensity = isHover ? 100000.0f : 5000.0f;
	float curIntensity = spotLight->Intensity;
	curIntensity = FMath::Lerp( curIntensity , targetIntensity , deltaTime * 5.0f );
	spotLight->SetIntensity( curIntensity );

	if ( spotConeDynMat )
	{
		float targetBrightness = isHover ? 10.0f : 1.0f;
		curConeBrightness = FMath::Lerp( curConeBrightness , targetBrightness , deltaTime * 5.0f );
		spotConeDynMat->SetScalarParameterValue( TEXT( "EmissiveBrightness" ) , curConeBrightness );
	}

	if ( FMath::IsNearlyEqual( curIntensity , targetIntensity , 0.001f ) )
	{
		isPlayIntensityAnim = false;
	}
}

// Called every frame
void ASelectRoleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	updateSpotLightIntensity( DeltaTime );
}

void ASelectRoleActor::OnHover_Implementation()
{
	isHover = true;
	isPlayIntensityAnim = true;
}

void ASelectRoleActor::OnUnhover_Implementation()
{
	isHover = false;
	isPlayIntensityAnim = true;
}

void ASelectRoleActor::OnPress_Implementation( AMotionControllerBase *hand )
{

}

void ASelectRoleActor::OnRelease_Implementation( AMotionControllerBase *hand )
{

}

