// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/MMOPlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"


#if WITH_EDITOR
#include "DrawDebugHelpers.h"
#endif

void FMMOParabolaCameraParams::UpdateInitialParams()
{
	ZoomClamp.X = InverseComputeParabola(ZoomClampDistance.X);
	ZoomClamp.Y = InverseComputeParabola(ZoomClampDistance.Y);

	CurrentZoomDistance = InverseComputeParabola(InitialZoomLength);
	CurrentZoomDistance = FMath::Clamp(CurrentZoomDistance, ZoomClamp.X, ZoomClamp.Y);
	TargetZoomDistance = CurrentZoomDistance;
}

void FMMOParabolaCameraParams::UpdateZoomDistance(float DeltaSeconds, float AxisValue)
{
	TargetZoomDistance += DeltaSeconds * ZoomSpeed * AxisValue;
	TargetZoomDistance = FMath::Clamp(TargetZoomDistance, ZoomClamp.X, ZoomClamp.Y);

	if (FMath::IsNearlyEqual(TargetZoomDistance, CurrentZoomDistance, 0.1f))
	{
		CurrentZoomDistance = TargetZoomDistance;
	}
	else
	{
		// smooth interpolation
		CurrentZoomDistance = FMath::FInterpTo(CurrentZoomDistance, TargetZoomDistance, DeltaSeconds, 3.f);
	}
}

AMMOPlayerCameraManager::AMMOPlayerCameraManager()
	: Super()
{
}

void AMMOPlayerCameraManager::BeginPlay()
{
	Super::BeginPlay();

	ParabolaParameters.UpdateInitialParams(); // probably uncesessary
}

void AMMOPlayerCameraManager::UpdateViewTargetInternal(FTViewTarget& OutVT, float DeltaTime)
{
	Super::UpdateViewTargetInternal(OutVT, DeltaTime);

	if (ViewTarget.Target.Get() == nullptr)
		return;
	
	// Handle Zoom
	ParabolaParameters.UpdateZoomDistance(DeltaTime, AccCameraZoom);
	AccCameraZoom = 0.f;

	// Internal compute POV. POV is express as offset from ViewTarget.
	UpdateCurrentOffset();

	OutVT.POV.Location = ViewTarget.Target->GetActorLocation() + CurrentOffset;
	OutVT.POV.Rotation = FRotationMatrix::MakeFromX(ViewTarget.Target->GetActorLocation() - OutVT.POV.Location).Rotator();

#if WITH_EDITORONLY_DATA
	if (bDebug)
	{
		const FVector ViewTargetLoc = GetViewTarget()->GetActorLocation();

		FVector LastPoint = ViewTargetLoc;

		constexpr float XStep = 25.f;
		// draw parable
		for (int32 i = 0; i < 100; i++)
		{
			FVector Delta;
			Delta.X = -XStep * (float)i;
			Delta.Y = 0.f;
			Delta.Z = ParabolaParameters.ComputeParabola(Delta.X * 0.01f);

			FVector NewPoint = ViewTargetLoc + Delta;

			DrawDebugLine(GetWorld(), LastPoint, NewPoint, FColor::Red, false, -1.f, 0, 5.f);

			LastPoint = NewPoint;
		}
	}
#endif
}

void AMMOPlayerCameraManager::UpdateCurrentOffset()
{
	// X offset
	CurrentOffset = ViewTarget.Target->GetActorForwardVector() * (-ParabolaParameters.GetZoomDistance()) * 100.f;

	// Z offset
	CurrentOffset += ParabolaParameters.GetZoomLength() * FVector::UpVector;
}

void AMMOPlayerCameraManager::CameraZoom(float AxisValue)
{
	// will be processed inside Tick()
	AccCameraZoom += AxisValue;
}

#if WITH_EDITORONLY_DATA
void AMMOPlayerCameraManager::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	ParabolaParameters.UpdateInitialParams();
}
#endif

AMMOPlayerCameraManager* AMMOPlayerCameraManager::GetPlayerCameraManager(const UObject* WorldContextObject, int32 PlayerIndex)
{
	return Cast<AMMOPlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(WorldContextObject, PlayerIndex));
}
