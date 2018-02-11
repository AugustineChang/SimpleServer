// Copyright 2017-2020 nasuVR All Rights Reserved.

#include "SettingWidgetActor.h"
#include "kismet/GameplayStatics.h"
#include "kismet/KismetMathLibrary.h"
#include "Components/WidgetComponent.h"
#include "Sound/SoundWave.h"
#include "PawnAndHand/MotionControllerBase.h"


// Sets default values
ASettingWidgetActor::ASettingWidgetActor() : widgetRadius( 15.0f ) , widgetToCamera( FVector::ForwardVector ) , 
isShowWidget( false ) , isPlayScaleWidgetAnim( false ) , savedScale( 0.0f ) , delayTimer( 0.0f )
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	originPoint = CreateDefaultSubobject<USceneComponent>( TEXT( "OriginPoint" ) );
	RootComponent = originPoint;

	WidgetPoint = CreateDefaultSubobject<USceneComponent>( TEXT( "WidgetPoint" ) );
	WidgetPoint->SetupAttachment( RootComponent );

	clickMesh = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "ClickMesh" ) );
	clickMesh->SetupAttachment( WidgetPoint );

	menuWidget = CreateDefaultSubobject<UWidgetComponent>( TEXT( "MenuWidget" ) );
	menuWidget->SetupAttachment( WidgetPoint );
}

// Called when the game starts or when spawned
void ASettingWidgetActor::BeginPlay()
{
	Super::BeginPlay();
	
	menuWidget->SetVisibility( isShowWidget );
	savedScale = menuWidget->RelativeScale3D.X;
	menuWidget->SetRelativeScale3D( FVector::ZeroVector );

	clickMesh->OnComponentBeginOverlap.AddDynamic( this , &ASettingWidgetActor::OnClickWidgetBtn );
}

void ASettingWidgetActor::updateHandWidgetRotation( float deltaTime )
{
	APlayerCameraManager *cameraManager = UGameplayStatics::GetPlayerCameraManager( this , 0 );
	FVector cameraLocInWidget = GetActorTransform().InverseTransformPosition( cameraManager->GetCameraLocation() );
	cameraLocInWidget = UKismetMathLibrary::ProjectVectorOnToPlane( cameraLocInWidget , FVector::RightVector );
	cameraLocInWidget.Normalize();

	if ( !widgetToCamera.Equals( cameraLocInWidget , 0.001f ) )
	{
		widgetToCamera = FMath::Lerp( widgetToCamera , cameraLocInWidget , deltaTime * 5.0f );
		widgetToCamera.Normalize();

		WidgetPoint->SetRelativeLocation( widgetToCamera * widgetRadius );
		FRotator relativeRot = UKismetMathLibrary::MakeRotFromXY( widgetToCamera , FVector::RightVector );
		WidgetPoint->SetRelativeRotation( relativeRot );
	}
}

void ASettingWidgetActor::updateHandWidgetScale( float deltaTime )
{
	if ( !isPlayScaleWidgetAnim ) return;

	float targetScale = isShowWidget ? savedScale : 0.0f;
	float curScale = menuWidget->RelativeScale3D.X;
	float lerpScale = FMath::Lerp( curScale , targetScale , deltaTime * 10.0f );
	menuWidget->SetRelativeScale3D( FVector::OneVector * lerpScale );

	if ( FMath::IsNearlyEqual( lerpScale , targetScale , 0.001f ) )
	{
		isPlayScaleWidgetAnim = false;
		if ( !isShowWidget )
		{
			menuWidget->SetVisibility( false );
		}
	}
}

void ASettingWidgetActor::OnClickWidgetBtn( UPrimitiveComponent* OverlappedComp , AActor* OtherActor , UPrimitiveComponent* OtherComp , int32 OtherBodyIndex , bool bFromSweep , const FHitResult& SweepResult )
{
	AMotionControllerBase *controller = Cast<AMotionControllerBase>( OtherActor );
	if ( !controller ) return;
	if ( delayTimer <= 0.5f ) return;
	delayTimer = 0.0f;

	isShowWidget = !isShowWidget;
	isPlayScaleWidgetAnim = true;

	if ( isShowWidget )
	{
		menuWidget->SetVisibility( true );
	}

	if ( clickSound )
	{
		UGameplayStatics::PlaySoundAtLocation( this , clickSound , clickMesh->GetComponentLocation() );
	}
	controller->rumbleController( 0.7f );
}

// Called every frame
void ASettingWidgetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	delayTimer += DeltaTime;

	updateHandWidgetRotation( DeltaTime );
	updateHandWidgetScale( DeltaTime );
}

FVector ASettingWidgetActor::getWidgetNormal()
{
	return menuWidget->GetForwardVector();
}

