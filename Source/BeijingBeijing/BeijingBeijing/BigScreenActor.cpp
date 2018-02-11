// Copyright 2017-2020 nasuVR All Rights Reserved.

#include "BigScreenActor.h"
#include "Other/DataTables.h"
#include "kismet/KismetMathLibrary.h"
#include "Components/WidgetComponent.h"
#include "GameControl/VRGameInstance.h"
#include "GameControl/GlobalMessageDispatcher.h"

// Sets default values
ABigScreenActor::ABigScreenActor() : distance( -150.0f , -80.0f ) , rowNum( 3 ) , startOffset( 280.0f , 100.0f , 0.0f ) ,
widgetsNum( 0 ) , cellDrawSize( 150.0f , 80.0f ) , cellTimer( 0.0f )
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	root = CreateDefaultSubobject<USceneComponent>( TEXT( "Root" ) );
	RootComponent = root;

	bigScreenHead = CreateDefaultSubobject<UWidgetComponent>( TEXT( "BigScreenHead" ) );
	bigScreenHead->SetupAttachment( RootComponent );
	bigScreenHead->SetDrawSize( FVector2D( 800.0f , 80.0f ) );
	bigScreenHead->SetRelativeLocation( FVector( 5.0f , 0.0f , 185.0f ) );

	bigScreenBody = CreateDefaultSubobject<UWidgetComponent>( TEXT( "BigScreenBody" ) );
	bigScreenBody->SetupAttachment( RootComponent );
	bigScreenBody->SetDrawSize( FVector2D( 800.0f , 450.0f ) );
}

// Called when the game starts or when spawned
void ABigScreenActor::BeginPlay()
{
	Super::BeginPlay();
	
	if ( headClass.Get() != nullptr )
	{
		bigScreenHead->SetWidgetClass( headClass );
		bigScreenHead->InitWidget();
	}

	if ( bodyClass.Get() != nullptr )
	{
		bigScreenBody->SetWidgetClass( bodyClass );
		bigScreenBody->InitWidget();
		UBigScreenWidget *screenWidget = Cast<UBigScreenWidget>( bigScreenBody->GetUserWidgetObject() );
		screenWidget->initWidgetActor( this );
	}

	bool hasMessager;
	UGlobalMessageDispatcher *messager = UGlobalMessageDispatcher::GetGlobalMessager( hasMessager );
	if ( hasMessager )
	{
		messager->DoCustomAction.AddDynamic( this , &ABigScreenActor::OnHoverScenicSpot );
		messager->DoCustomAction.AddDynamic( this , &ABigScreenActor::OnClickBigScreenCell );
	}

	GetWorld()->GetTimerManager().SetTimer( timerHandle , this , &ABigScreenActor::updateScreenTime , 0.5f , true );
	updateWeatherData();
}

void ABigScreenActor::EndPlay( const EEndPlayReason::Type EndPlayReason )
{
	Super::EndPlay( EndPlayReason );

	GetWorld()->GetTimerManager().ClearTimer( timerHandle );
}

void ABigScreenActor::createWidgetByTable()
{
	UVRGameInstance *gameInstance = Cast<UVRGameInstance>( GetGameInstance() );
	if ( !gameInstance ) return;

	UTable *screenTable;
	if ( !gameInstance->getTableData( TEXT( "BigScreenTable" ) , screenTable ) ) return;

	TArray<FString> rowNames = screenTable->getRowNames();
	for ( const FString& oneName : rowNames )
	{
		FString compName = FString( TEXT( "BigScreenCell" ) ) + FString::FromInt( widgetsNum );
		UWidgetComponent *uiComp = NewObject<UWidgetComponent>( this , *compName );
		uiComp->SetupAttachment( RootComponent );
		uiComp->RegisterComponent();
		uiComp->SetDrawSize( cellDrawSize );
		uiComp->TranslucencySortPriority = 1;
		uiComp->SetCollisionProfileName( FName( TEXT( "LaserPreset" ) ) );

		if ( cellClass.Get() != nullptr )
		{
			uiComp->SetWidgetClass( cellClass );
			uiComp->InitWidget();
		}

		FVector uiLoc;
		uiLoc.Y = distance.X * ( widgetsNum % rowNum ) + startOffset.X;
		uiLoc.Z = distance.Y * ( widgetsNum / rowNum ) + startOffset.Y;
		uiLoc.X = startOffset.Z;
		uiComp->SetRelativeLocation( uiLoc );

		UBigScreenCellWidget *cellWidget = Cast<UBigScreenCellWidget>( uiComp->GetUserWidgetObject() );
		if ( cellWidget )
		{
			cellWidget->createCell( widgetsNum , oneName );
		}

		widgetsList.Add( uiComp );
		widgetsNum++;
	}
}

void ABigScreenActor::updateScreenCellsAlpha( float deltaTime )
{
	if ( !isPlayCellsAlpha ) return;
	if ( widgetsNum <= 0 ) return;
	
	cellTimer += deltaTime;
	float waitTime = isShowCell ? 0.8f : 0.0f;
	if ( cellTimer >= waitTime )
	{
		cellTimer = 0.0f;
		isPlayCellsAlpha = false;

		if ( !isShowCell )
		{
			for ( UWidgetComponent *widget : widgetsList )
			{
				widget->SetVisibility( false );
				widget->SetCollisionEnabled( ECollisionEnabled::NoCollision );
			}
		}
		else
		{
			for ( UWidgetComponent *widget : widgetsList )
			{
				widget->SetVisibility( true );
				widget->SetCollisionEnabled( ECollisionEnabled::QueryOnly );
			}
		}
	}
}

void ABigScreenActor::updateScreenTime()
{
	UBigScreenHeadWidget *headWidget = Cast<UBigScreenHeadWidget>( bigScreenHead->GetUserWidgetObject() );
	headWidget->updateDataTime( UKismetMathLibrary::Now().ToString( TEXT( "%H:%M:%S" ) ) );
}

void ABigScreenActor::updateWeatherData()
{
	FWeatherData weather;
	UWeatherRequest::getSingleton()->getWeatherData( EWeatherCityEnum::beijing , 0 , weather );

	UBigScreenHeadWidget *headWidget = Cast<UBigScreenHeadWidget>( bigScreenHead->GetUserWidgetObject() );
	headWidget->updateWeather( weather );
}

void ABigScreenActor::OnHoverScenicSpot( const FString &cmd , const FString &paramStr , bool isNetworkSend )
{
	if ( isSelectCell ) return;
	if ( cmd != TEXT( "SandBoxHover" ) ) return;

	FString spotKey = paramStr;
	if ( spotKey == TEXT( "NoKey" ) )//unhover
	{
		isHoverSpot = false;
		isPlayCellsAlpha = true;
		isShowCell = true;

		UBigScreenWidget *screenWidget = Cast<UBigScreenWidget>( bigScreenBody->GetUserWidgetObject() );
		screenWidget->OnScenicSpotHoverState( false , nullptr );
	}
	else//hover
	{
		isHoverSpot = true;
		isPlayCellsAlpha = true;
		isShowCell = false;

		UTableRow *spotTable = nullptr;
		UVRGameInstance *gameInstance = Cast<UVRGameInstance>( GetGameInstance() );
		if ( gameInstance->getTableData( TEXT( "ScenicSpotTable" ) , spotKey , spotTable ) )
		{
			UBigScreenWidget *screenWidget = Cast<UBigScreenWidget>( bigScreenBody->GetUserWidgetObject() );
			screenWidget->OnScenicSpotHoverState( true , spotTable );
		}
	}
}

void ABigScreenActor::OnClickBigScreenCell( const FString &cmd , const FString &paramStr , bool isNetworkSend )
{
	if ( cmd != TEXT( "BigScreenClick" ) ) return;

	FString screenKey = paramStr;
	if ( screenKey == TEXT( "NoKey" ) )//cancel click
	{
		isSelectCell = false;

		isPlayCellsAlpha = true;
		isShowCell = true;

		UBigScreenWidget *screenWidget = Cast<UBigScreenWidget>( bigScreenBody->GetUserWidgetObject() );
		screenWidget->OnTourRouteState( false , nullptr );
	}
	else//click cell
	{
		isSelectCell = true;
		
		isPlayCellsAlpha = true;
		isShowCell = false;

		UTableRow *screenTable = nullptr;
		UVRGameInstance *gameInstance = Cast<UVRGameInstance>( GetGameInstance() );
		if ( gameInstance->getTableData( TEXT( "BigScreenTable" ) , screenKey , screenTable ) )
		{
			UBigScreenWidget *screenWidget = Cast<UBigScreenWidget>( bigScreenBody->GetUserWidgetObject() );
			screenWidget->OnTourRouteState( true , screenTable );
		}
	}
}

// Called every frame
void ABigScreenActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	updateScreenCellsAlpha( DeltaTime );
}

