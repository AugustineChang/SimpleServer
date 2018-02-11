// Fill out your copyright notice in the Description page of Project Settings.

#include "LaserActor.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Camera/PlayerCameraManager.h"
#include "kismet/KismetMaterialLibrary.h"
#include "kismet/KismetMathLibrary.h"
#include "kismet/GameplayStatics.h"
#include "Interface/LaserInterface.h"
#include "PawnAndHand/MotionControllerBase.h"
#include "GameControl/GlobalMessageDispatcher.h"
#include "Components/WidgetInteractionComponent.h"

// Sets default values
ALaserActor::ALaserActor() : indices( { 0,1,2,1,3,2 } ) , isLackOfData( false ) , EndPointOffset( FVector( -1.0f , 0.0f , 0.0f ) ) ,
endPointRotByNormal( true ) , distanceToFade( 50.0f ) , laserWidth( 2.0f ) , laserEnable( { true,false } ) ,
uvs( { FVector2D() , FVector2D(1.0f,0.0f) , FVector2D(0.0f,1.0f) , FVector2D(1.0f,1.0f) } ) , isLaserLeft( true )
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	laserRoot = CreateDefaultSubobject<USceneComponent>( TEXT( "LaserRoot" ) );
	RootComponent = laserRoot;

	laserMesh = CreateDefaultSubobject<UProceduralMeshComponent>( TEXT( "LaserMesh" ) );
	laserMesh->SetupAttachment( RootComponent );

	laserEndPoint = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "LaserEndPoint" ) );
	laserEndPoint->SetupAttachment( RootComponent );

	widgetHitComp = CreateDefaultSubobject<UWidgetInteractionComponent>( TEXT( "WigdetHit" ) );
	widgetHitComp->SetupAttachment( RootComponent );
	widgetHitComp->TraceChannel = UEngineTypes::ConvertToCollisionChannel( TraceTypeQuery3 );
}

// Called when the game starts or when spawned
void ALaserActor::BeginPlay()
{
	Super::BeginPlay();
	
	ignoredActors.Add( this );
	ignoredActors.Add( GetAttachParentActor() );

	laserEndPoint->SetVisibility( false );
	if ( laserMaterial )
	{
		laserDynMat = UKismetMaterialLibrary::CreateDynamicMaterialInstance( this , laserMaterial );
		laserEndDynMat = laserEndPoint->CreateDynamicMaterialInstance( 0 );

		SetLaserMode( LaserModeType::Close );

		handActor = Cast<AMotionControllerBase>( GetParentActor() );
		if ( handActor )
		{
			isLaserLeft = handActor->chirality == EControllerHand::Left;
		}

		initActionBind();
		updatePlayerIndex( 0 , isLaserLeft );
	}
	else isLackOfData = true;
}

FVector ALaserActor::getCameraLocation()
{
	APlayerCameraManager *cameraManager = UGameplayStatics::GetPlayerCameraManager( this , 0 );
	return cameraManager->GetCameraLocation();
}

void ALaserActor::laserTrace()
{
	FVector start = laserRoot->GetComponentLocation();
	FVector end = start + laserRoot->GetForwardVector() * laserMaxDistance;
	
	isTraceHit = UKismetSystemLibrary::LineTraceSingle( this , start , end , TraceTypeQuery3 , false , ignoredActors , EDrawDebugTrace::None , hitResult , true );

	if ( isTraceHit )
	{
		float realDist = ( hitResult.ImpactPoint - start ).Size();
		laserDistance = realDist;

		if ( !laserEndPoint->IsVisible() )
			laserEndPoint->SetVisibility( true );
	}
	else
	{
		if ( laserMode == LaserModeType::Always )
		{
			laserDistance = laserMaxDistance;
		}

		if ( laserEndPoint->IsVisible() )
			laserEndPoint->SetVisibility( false );
	}
}

void ALaserActor::laserCheckInterface()
{
	if ( handActor )
	{
		bool isLocal = handActor->networkHandComp->getIsLocalControl();
		if ( !isLocal ) return;
	}

	if ( isTraceHit )
	{
		AActor *curHitActor = hitResult.Actor.Get();
		if ( curHitActor != nullptr )
		{
			bool hasInterface = curHitActor->GetClass()->ImplementsInterface( ULaserInterface::StaticClass() );
			if ( !hasInterface ) curHitActor = nullptr;
		}

		if ( lastHitActor != curHitActor )
		{
			if ( lastHitActor != nullptr ) ILaserInterface::Execute_OnUnhover( lastHitActor );
			lastHitActor = curHitActor;
			if ( curHitActor != nullptr ) ILaserInterface::Execute_OnHover( curHitActor );
		}
	}
	else
	{
		if ( lastHitActor != nullptr ) ILaserInterface::Execute_OnUnhover( lastHitActor );
		lastHitActor = nullptr;
	}
}

void ALaserActor::GenerateVertex()
{
	vertices.Reset();

	for ( int32 i = 0; i <= 3; ++i )
	{
		FVector tempVec;
		tempVec.X = i < 2 ? 0.0f : laserDistance;
		tempVec.Y = i % 2 == 0 ? laserWidth * -0.5f : laserWidth * 0.5f;
		tempVec.Z = 0.0f;
		vertices.Add( tempVec );
	}

	int32 numSections = laserMesh->GetNumSections();
	if ( numSections <= 0 )
	{
		laserMesh->CreateMeshSection( 0 , vertices , indices , normals , uvs , vertexColors , tangents , false );
		laserMesh->SetMaterial( 0 , laserDynMat );
	}
	else
	{
		laserMesh->UpdateMeshSection( 0 , vertices , normals , uvs , vertexColors , tangents );
	}
}

void ALaserActor::UpdateLaserRotation()
{
	FVector cameraLocInLaser = GetActorTransform().InverseTransformPosition( getCameraLocation() );
	laserToCameraDir = cameraLocInLaser - FVector::ForwardVector * laserDistance * 0.5f;

	FRotator relativeRot = UKismetMathLibrary::MakeRotFromXZ( FVector::ForwardVector , FVector( 0.0f , laserToCameraDir.Y , laserToCameraDir.Z ) );
	laserMesh->SetRelativeRotation( relativeRot );
}

void ALaserActor::UpdateLaserEndPoint()
{
	if ( !laserEndPoint->IsVisible() ) return;

	laserEndPoint->SetRelativeLocation( EndPointOffset + FVector::ForwardVector*laserDistance );
	if ( endPointRotByNormal )
	{
		laserEndPoint->SetWorldRotation( UKismetMathLibrary::MakeRotFromZ( hitResult.ImpactNormal ) );
	}
	else
	{
		laserEndPoint->SetRelativeRotation( UKismetMathLibrary::MakeRotFromZ( laserToCameraDir ) );
	}
}

void ALaserActor::CalcLaserFade( float deltatime )
{
	float alpha = 0.0f;
	if ( isTraceHit )
	{
		alpha = ( laserMaxDistance - laserDistance ) / ( laserMaxDistance - distanceToFade );
		alpha = FMath::Clamp( alpha , 0.2f , 1.0f );
	}
	else
	{
		if ( laserMode == LaserModeType::Always ) alpha = 1.0f;
	}
	
	laserAlpha = FMath::Lerp( laserAlpha , alpha , deltatime *10.0f );

	bool isVisible = laserAlpha > 0.001f;
	if ( isVisible )
	{
		laserDynMat->SetScalarParameterValue( TEXT( "Fade Control" ) , laserAlpha );
	}
	
	if ( laserMesh->IsVisible() != isVisible )
	{
		laserMesh->SetVisibility( isVisible );
	}
}

void ALaserActor::initActionBind()
{
	if ( handActor )
	{
		bool isLocal = handActor->networkHandComp->getIsLocalControl();
		if ( !isLocal ) return;
	}
	
	bool hasMessager;
	UGlobalMessageDispatcher *messager = UGlobalMessageDispatcher::GetGlobalMessager( hasMessager );
	if ( !hasMessager ) return;

	if ( isLaserLeft )
	{
		messager->OnLeftTrigger_Pressed.AddDynamic( this , &ALaserActor::OnTriggerPressed );
		messager->OnLeftTrigger_Released.AddDynamic( this , &ALaserActor::OnTriggerReleased );
	}
	else
	{
		messager->OnRightTrigger_Pressed.AddDynamic( this , &ALaserActor::OnTriggerPressed );
		messager->OnRightTrigger_Released.AddDynamic( this , &ALaserActor::OnTriggerReleased );
	}
}

void ALaserActor::OnTriggerPressed()
{
	if ( lastHitActor != nullptr )
	{
		ILaserInterface::Execute_OnPress( lastHitActor , handActor );
		pressedActor = lastHitActor;
	}

	widgetHitComp->PressPointerKey( EKeys::LeftMouseButton );
}

void ALaserActor::OnTriggerReleased()
{
	if ( pressedActor != nullptr )
	{
		ILaserInterface::Execute_OnRelease( pressedActor , handActor );
		pressedActor = nullptr;
	}

	widgetHitComp->ReleasePointerKey( EKeys::LeftMouseButton );
}

// Called every frame
void ALaserActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if ( isLackOfData || laserMode == LaserModeType::Close )
	{
		return;
	}

	laserTrace();
	laserCheckInterface();
	GenerateVertex();
	UpdateLaserRotation();
	UpdateLaserEndPoint();
	CalcLaserFade( DeltaTime );
}

void ALaserActor::updatePlayerIndex( int32 playerIndex , bool isLeft )
{
	float offset = isLeft ? 0.0f : 1.0f;
	widgetHitComp->PointerIndex = playerIndex * 4.0f + offset;
	widgetHitComp->VirtualUserIndex = playerIndex;
}

void ALaserActor::SetLaserMode( LaserModeType type )
{
	if ( laserMode == type )return;

	laserMode = type;
	if ( laserMode == LaserModeType::Nomral )
	{
		laserDynMat->SetVectorParameterValue( TEXT( "Emissive Color" ) , 
			FLinearColor( 0.0f , 0.3515f , 1.0f ) );
		laserEndDynMat->SetVectorParameterValue( TEXT( "Center Emissive Color" ) , 
			FLinearColor( 1047.7718f , 51.1774f , 15000.0f ) );
		laserEndDynMat->SetVectorParameterValue( TEXT( "Emissive Color" ) ,
			FLinearColor( 0.12744f , 0.2462f , 0.9046f ) );
		laserEndDynMat->SetVectorParameterValue( TEXT( "Lv2 Emissive Color" ) ,
			FLinearColor( 0.3997f , 0.2854f , 2.0f ) );

		laserMaxDistance = 150.0f;
		widgetHitComp->InteractionSource = EWidgetInteractionSource::World;
		widgetHitComp->InteractionDistance = 150.0f;
	}
	else if ( laserMode == LaserModeType::Always )
	{
		laserDynMat->SetVectorParameterValue( TEXT( "Emissive Color" ) ,
			FLinearColor( 1.0f , 0.2122f , 0.1441f ) );
		laserEndDynMat->SetVectorParameterValue( TEXT( "Center Emissive Color" ) ,
			FLinearColor( 15000.0f , 76.1774f , 51.1774f ) );
		laserEndDynMat->SetVectorParameterValue( TEXT( "Emissive Color" ) ,
			FLinearColor( 0.9046f , 0.1912f , 0.1274f ) );
		laserEndDynMat->SetVectorParameterValue( TEXT( "Lv2 Emissive Color" ) ,
			FLinearColor( 2.0f , 0.4248f , 0.2854f ) );

		laserMaxDistance = 3000.0f;
		widgetHitComp->InteractionSource = EWidgetInteractionSource::World;
		widgetHitComp->InteractionDistance = 3000.0f;
	}
	else
	{
		if ( laserMesh->GetNumSections() > 0 )
		{
			laserMesh->ClearAllMeshSections();
		}

		if ( laserEndPoint->IsVisible() )
		{
			laserEndPoint->SetVisibility( false );
		}

		widgetHitComp->InteractionSource = EWidgetInteractionSource::Custom;
	}
}

void ALaserActor::SetLaserEnable( int32 index , bool isEnable )
{
	if ( index < 0 || index >= 2 )return;
	
	bool oldEnable = GetLaserEnable();
	laserEnable[index] = isEnable;
	bool newEnable = GetLaserEnable();

	if ( oldEnable != newEnable )
	{
		if ( newEnable )
		{
			SetLaserMode( savedLaserMode );
		}
		else
		{
			savedLaserMode = GetLaserMode();
			SetLaserMode( LaserModeType::Close );
		}
	}
}

bool ALaserActor::GetLaserEnable()
{
	bool result = true;
	for ( int32 i = 0; i < laserEnable.Num(); ++i )
	{
		result = result && laserEnable[i];
		if ( !result ) break;
	}
	return result;
}
