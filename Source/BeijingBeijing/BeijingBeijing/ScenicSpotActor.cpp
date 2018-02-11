// Copyright 2017-2020 nasuVR All Rights Reserved.

#include "ScenicSpotActor.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "GameControl/GlobalMessageDispatcher.h"
#include "kismet/KismetMathLibrary.h"
#include "kismet/GameplayStatics.h"
#include "Sound/SoundWave.h"
#include "PawnAndHand/VRCharacterBase.h"
#include "Network/NetworkComponent/NetworkAvatarComponent.h"

// Sets default values
AScenicSpotActor::AScenicSpotActor() : hoverScale( 1.5f ) , scaleAnimeSpeed( 5.0f ) , isLocallyControl( true ) , 
levelName()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	root = CreateDefaultSubobject<USceneComponent>( TEXT( "Root" ) );
	RootComponent = root;

	collisionBox = CreateDefaultSubobject<UBoxComponent>( TEXT( "CollisionBox" ) );
	collisionBox->SetupAttachment( RootComponent );

	scalePoint = CreateDefaultSubobject<USceneComponent>( TEXT( "ScalePoint" ) );
	scalePoint->SetupAttachment( RootComponent );

	spotMesh = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "SpotMesh" ) );
	spotMesh->SetupAttachment( scalePoint );

	spotUI = CreateDefaultSubobject<UWidgetComponent>( TEXT( "SpotUI" ) );
	spotUI->SetupAttachment( scalePoint );
}

// Called when the game starts or when spawned
void AScenicSpotActor::BeginPlay()
{
	Super::BeginPlay();

	AVRCharacterBase *pawn = Cast<AVRCharacterBase>( UGameplayStatics::GetPlayerPawn( this , 0 ) );
	if ( pawn )
	{
		isLocallyControl = pawn->networkPlayer->IsAvatarGuide();
	}

	if ( !isLocallyControl )
	{
		bool hasMessager;
		UGlobalMessageDispatcher *messager = UGlobalMessageDispatcher::GetGlobalMessager( hasMessager );
		if ( hasMessager )
		{
			messager->OnGetCustomAction.AddDynamic( this , &AScenicSpotActor::OnGetCustomAction );
		}
	}

	initSpotActorByTable();
	initSpotActorWeather();
}

void AScenicSpotActor::initSpotActorByTable()
{
	UVRGameInstance *gameInstance = Cast<UVRGameInstance>( GetGameInstance() );
	if ( !gameInstance ) return;

	UTableRow *spotTable;
	if ( !gameInstance->getTableData( TEXT( "ScenicSpotTable" ) , spotKey , spotTable ) ) return;

	FString outName;
	if ( spotTable->getRowData( TEXT( "name" ) , outName ) )
	{
		OnGetTableData( outName );
	}

	spotTable->getRowData( TEXT( "mapName" ) , levelName );
}

void AScenicSpotActor::initSpotActorWeather()
{
	FWeatherData weather;
	UWeatherRequest::getSingleton()->getWeatherData( weatherSpot , 0 , weather );

	OnGetWeatherData( weather );
}

void AScenicSpotActor::updateSpotScale( float deltaTime )
{
	if ( !isStartHoverAnime ) return;

	float targetScale = isHover ? hoverScale : 1.0f;
	float curScale = scalePoint->GetComponentScale().X;
	if ( FMath::IsNearlyEqual( curScale , targetScale , 0.001f ) )
	{
		isStartHoverAnime = false;
		return;
	}

	float lerpScale = FMath::Lerp( curScale , targetScale , deltaTime * scaleAnimeSpeed );
	scalePoint->SetWorldScale3D( FVector::OneVector * lerpScale );
}

void AScenicSpotActor::updateBillboardUI( float deltaTime )
{
	APlayerCameraManager *cameraManager = UGameplayStatics::GetPlayerCameraManager( this , 0 );
	FVector cameraLocInSpot = GetActorTransform().InverseTransformPosition( cameraManager->GetCameraLocation() );
	cameraLocInSpot = cameraLocInSpot - spotUI->RelativeLocation;
	cameraLocInSpot = FVector::VectorPlaneProject( cameraLocInSpot , FVector::UpVector );

	FRotator relativeRot = UKismetMathLibrary::MakeRotFromX( cameraLocInSpot );
	spotUI->SetRelativeRotation( relativeRot );
}

void AScenicSpotActor::OnGetCustomAction( const FString &cmd , const FString &paramStr )
{
	if ( !cmd.Equals( TEXT( "SandBoxHover" ) ) ) return;

	FString spotKey = paramStr;
	if ( spotKey == TEXT( "NoKey" ) )//unhover
	{
		unhoverAction();
	}
	else//hover
	{
		hoverAction();
	}
}

void AScenicSpotActor::hoverAction()
{
	isHover = true;
	isStartHoverAnime = true;

	if ( hoverSound )
	{
		UGameplayStatics::PlaySoundAtLocation( this , hoverSound , GetActorLocation() );
	}
}

void AScenicSpotActor::unhoverAction()
{
	isHover = false;
	isStartHoverAnime = true;
}

// Called every frame
void AScenicSpotActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	updateSpotScale( DeltaTime );
	updateBillboardUI( DeltaTime );
}

void AScenicSpotActor::OnHover_Implementation()
{
	hoverAction();

	bool hasMessager;
	UGlobalMessageDispatcher *messager = UGlobalMessageDispatcher::GetGlobalMessager( hasMessager );
	if ( hasMessager )
	{
		messager->DoCustomAction.Broadcast( TEXT( "SandBoxHover" ) , spotKey , true );
	}
}

void AScenicSpotActor::OnUnhover_Implementation()
{
	unhoverAction();

	bool hasMessager;
	UGlobalMessageDispatcher *messager = UGlobalMessageDispatcher::GetGlobalMessager( hasMessager );
	if ( hasMessager )
	{
		messager->DoCustomAction.Broadcast( TEXT( "SandBoxHover" ) , TEXT( "NoKey" ) , true );
	}
}

void AScenicSpotActor::OnPress_Implementation( AMotionControllerBase *hand )
{
	if ( clickSound )
	{
		UGameplayStatics::PlaySoundAtLocation( this , clickSound , GetActorLocation() );
	}
}

void AScenicSpotActor::OnRelease_Implementation( AMotionControllerBase *hand )
{
	if ( levelName.IsEmpty() ) return;

	bool hasMessager;
	UGlobalMessageDispatcher *messager = UGlobalMessageDispatcher::GetGlobalMessager( hasMessager );
	if ( hasMessager )
	{
		messager->DoCustomAction.Broadcast( TEXT( "OpenLevel" ) , levelName , true );
	}

	AVRCharacterBase *pawn = Cast<AVRCharacterBase>( UGameplayStatics::GetPlayerPawn( this , 0 ) );
	pawn->networkPlayer->logoutSpace();

	UVRGameInstance *gameInstance = Cast<UVRGameInstance>( GetGameInstance() );
	gameInstance->isOpenLevel = true;

	UGameplayStatics::OpenLevel( this , FName( *levelName ) );
}

