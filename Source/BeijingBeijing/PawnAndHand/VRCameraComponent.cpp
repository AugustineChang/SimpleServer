// Fill out your copyright notice in the Description page of Project Settings.

#include "VRCameraComponent.h"
#include "IHeadMountedDisplay.h"


UVRCameraComponent::UVRCameraComponent( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer ) , IsUpdateRotation( true ) , IsUpdatePosition( false )
{
}

void UVRCameraComponent::GetCameraView( float DeltaTime , FMinimalViewInfo& DesiredView )
{
	if ( bLockToHmd && GEngine->HMDDevice.IsValid() && GEngine->HMDDevice->IsHeadTrackingAllowed() )
	{
		const FTransform ParentWorld = CalcNewComponentToWorld( FTransform() );
		GEngine->HMDDevice->SetupLateUpdate( ParentWorld , this );
		
		if ( GEngine->HMDDevice->UpdatePlayerCamera( VROrientation , VRPosition ) )
		{
			FQuat OrientationCopy = VROrientation;
			FVector PositionCopy = VRPosition;

			if ( !IsUpdateRotation )
			{
				OrientationCopy = FQuat::MakeFromEuler( FVector::ZeroVector );
			}

			if ( !IsUpdatePosition )
			{
				PositionCopy.X = 0.0f;
				PositionCopy.Y = 0.0f;
				PositionCopy.Z -= 90.0f;
			}

			SetRelativeTransform( FTransform( OrientationCopy , PositionCopy ) );
		}
		else
		{
			ResetRelativeTransform();
		}
	}

	if ( bUsePawnControlRotation )
	{
		const APawn* OwningPawn = Cast<APawn>( GetOwner() );
		const AController* OwningController = OwningPawn ? OwningPawn->GetController() : nullptr;
		if ( OwningController && OwningController->IsLocalPlayerController() )
		{
			const FRotator PawnViewRotation = OwningPawn->GetViewRotation();
			if ( !PawnViewRotation.Equals( GetComponentRotation() ) )
			{
				SetWorldRotation( PawnViewRotation );
			}
		}
	}

	if ( bUseAdditiveOffset )
	{
		FTransform OffsetCamToBaseCam = AdditiveOffset;
		FTransform BaseCamToWorld = GetComponentToWorld();
		FTransform OffsetCamToWorld = OffsetCamToBaseCam * BaseCamToWorld;

		DesiredView.Location = OffsetCamToWorld.GetLocation();
		DesiredView.Rotation = OffsetCamToWorld.Rotator();
	}
	else
	{
		DesiredView.Location = GetComponentLocation();
		DesiredView.Rotation = GetComponentRotation();
	}

	DesiredView.FOV = bUseAdditiveOffset ? ( FieldOfView + AdditiveFOVOffset ) : FieldOfView;
	DesiredView.AspectRatio = AspectRatio;
	DesiredView.bConstrainAspectRatio = bConstrainAspectRatio;
	DesiredView.bUseFieldOfViewForLOD = bUseFieldOfViewForLOD;
	DesiredView.ProjectionMode = ProjectionMode;
	DesiredView.OrthoWidth = OrthoWidth;
	DesiredView.OrthoNearClipPlane = OrthoNearClipPlane;
	DesiredView.OrthoFarClipPlane = OrthoFarClipPlane;

	// See if the CameraActor wants to override the PostProcess settings used.
	DesiredView.PostProcessBlendWeight = PostProcessBlendWeight;
	if ( PostProcessBlendWeight > 0.0f )
	{
		DesiredView.PostProcessSettings = PostProcessSettings;
	}
}

FVector UVRCameraComponent::GetHMDInputLocation()
{
	return VRPosition;
}

FVector UVRCameraComponent::GetDeltaLocation()
{
	FVector Temp = VRPosition - LastVRPosition;
	LastVRPosition = VRPosition;
	return Temp;
}