// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "MMOPlayerCameraManager.generated.h"

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMOParabolaCameraParams
{
	GENERATED_BODY()
public:
	// ax2 + bx + c. A Controls the width of the parable. A = 0 flattens out the parabola.
	UPROPERTY(EditAnywhere, Category = Parabola)
	float A = 25.f;

	// ax2 + bx + c. B cause the parabola to move around. Should always kept == 0
	UPROPERTY(EditAnywhere, Category = Parabola)
	float B = 0.f;

	// ax2 + bx + c. C cause the parabola to move up or down. Pretty much a vertical offset.
	UPROPERTY(EditAnywhere, Category = Parabola)
	float C = 0.f;

	// Clamp on Y
	UPROPERTY(EditAnywhere, Category = Zoom)
	FVector2D ZoomClampDistance = FVector2D(1000.f, 3000.f);

	// m/s
	UPROPERTY(EditAnywhere, Category = Zoom)
	float ZoomSpeed = 20.f;

	// Called Once on BeginPlay, and everytime the object is rebuilt (editor-only)
	void UpdateInitialParams();

	// Called every tick.
	void UpdateZoomDistance(float DeltaSeconds, float AxisValue);
	
	// Get Y value.
	float GetZoomLength() const { return ComputeParabola(CurrentZoomDistance); }

	// Get X value.
	float GetZoomDistance() const { return CurrentZoomDistance; }

	FORCEINLINE float ComputeParabola(const float X) const
	{
		return A * X * X + B * X + C;
	}

	FORCEINLINE float InverseComputeParabola(const float Y) const
	{
		if ((C - Y) > 0.f)
		{
			UE_LOG(LogTemp, Error, TEXT("Inverse Compute parabola is not defined for this value"));
			return 0.f;
		}

		// this should give us 2 solutions +- sqrt.. we just pick 1.
		return (1.f / (2.f * A)) * (-B + FMath::Sqrt(B * B - 4.f * A * (C - Y)));
	}

protected:
	UPROPERTY(EditAnywhere, Category = Camera)
	float InitialZoomLength = 2000.f;

	UPROPERTY(VisibleInstanceOnly, Category = Camera)
	float CurrentZoomDistance = 2000.f;

	UPROPERTY(VisibleInstanceOnly, Category = Camera)
	float TargetZoomDistance = 2000.f;

	// Clamp on X. derived from ZoomClampDistance
	UPROPERTY(VisibleAnywhere, Category = Zoom)
	FVector2D ZoomClamp = FVector2D(0.f, 0.f);
};

UCLASS()
class MMO_SIMULATOR_API AMMOPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
public:
	AMMOPlayerCameraManager();

	UFUNCTION(BlueprintPure, Category = Camera, meta=(WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
	static AMMOPlayerCameraManager* GetPlayerCameraManager(const UObject* WorldContextObject, int32 PlayerIndex = 0);

	void CameraZoom(float AxisValue);

protected:
	virtual void BeginPlay() override;
	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;

	void UpdateCurrentOffset();

	// Modelling a Parabola (y = ax2 + bx + c). A = FVector2D.X, B = FVector2D.Y
	UPROPERTY(EditAnywhere, Category = Camera)
	FMMOParabolaCameraParams ParabolaParameters;

	UPROPERTY(VisibleInstanceOnly, Category = Camera)
	FVector CurrentOffset;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = Debug)
	bool bDebug = false;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
	float AccCameraZoom = 0.f;
};
