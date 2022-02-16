// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MMOPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Engine/World.h"
#include "Characters/MMOBaseHero.h"
#include "Core/MMOBaseHUD.h"

AMMOPlayerController::AMMOPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void AMMOPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Lock mouse to viewport
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	InputMode.SetHideCursorDuringCapture(false);

	SetInputMode(InputMode);
}

void AMMOPlayerController::SetSelectedHeroes(const TArray<AMMOBaseHero*>& InHeroes)
{
	// Deselect
	for (AMMOBaseHero* CurrentHero : SelectedHeroes)
	{
		CurrentHero->OnDeselected();
	}

	SelectedHeroes = InHeroes;

	// Select
	for (AMMOBaseHero* CurrentHero : SelectedHeroes)
	{
		CurrentHero->OnSelected();
	}
}

bool AMMOPlayerController::DeprojectMouseToTerrain(FVector& OutLocation, FVector& OutTerrainNormal) const
{
	FVector MouseLocation, Direction;
	if (DeprojectMousePositionToWorld(MouseLocation, Direction))
	{
		FCollisionObjectQueryParams Params(FCollisionObjectQueryParams::AllStaticObjects);

		FHitResult HitResult;
		if (GetWorld()->LineTraceSingleByObjectType(HitResult, MouseLocation, MouseLocation + Direction * 3000.f, Params))
		{
			OutLocation = HitResult.ImpactPoint;
			OutTerrainNormal = HitResult.ImpactNormal;
			return true;
		}
	}

	return false;
}

void AMMOPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (AMMODummyPawn* DummyPawn =  GetDummyPawn())
	{
		MoveCamera(DeltaTime);

		bHasValidMousePosition = GetMousePosition(CurrentMouseLocation.X, CurrentMouseLocation.Y);

		FVector WorldLocation, TerrainNormal;
		if(DeprojectMouseToTerrain(WorldLocation, TerrainNormal))
		{
			//DummyPawn->SetCursorLocationAndRotation(WorldLocation, TerrainNormal.Rotation());
		}
	}

	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();
	}
}

void AMMOPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &AMMOPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &AMMOPlayerController::OnSetDestinationReleased);

	InputComponent->BindAction("Select", IE_Pressed, this, &AMMOPlayerController::OnSelectPressed);
	InputComponent->BindAction("Select", IE_Released, this, &AMMOPlayerController::OnSelectReleased);
}

void AMMOPlayerController::MoveToMouseCursor()
{
	// Trace to see what is under the mouse cursor
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(Hit.ImpactPoint);
	}
}

void AMMOPlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	for (AMMOBaseHero* CurrentHero : SelectedHeroes)
	{
		float const Distance = FVector::Dist(DestLocation, CurrentHero->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if ((Distance > 50.0f))
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(CurrentHero->GetController(), DestLocation);
		}
	}
}

void AMMOPlayerController::ToggleCameraLock()
{
	bCameraLocked = !bCameraLocked;
}

void AMMOPlayerController::MoveCamera(float DeltaSeconds)
{
	APawn* const MyPawn = GetPawn();

	if(!MyPawn)
	{ 
		return;
	}

	if (bCameraLocked)
	{
		// #MMO_TODO
	}
	else
	{
		constexpr int32 BorderSize = 50;

		int32 SizeX, SizeY;
		GetViewportSize(SizeX, SizeY);

		float LocationX, LocationY;
		if (GetMousePosition(LocationX, LocationY))
		{
			const bool MoveUp = LocationY < BorderSize;
			const bool MoveDown = LocationY > (SizeY - BorderSize);
			const bool MoveLeft = LocationX < BorderSize;
			const bool MoveRight = LocationX > (SizeX - BorderSize);

			const float SpeedY = MoveUp ? CameraSpeed : MoveDown ? -CameraSpeed : 0.f;
			const float SpeedX = MoveRight ? CameraSpeed : MoveLeft ? -CameraSpeed : 0.f;

			const FVector Delta = FVector::ForwardVector * SpeedY + FVector::RightVector * SpeedX;
			MyPawn->AddActorWorldOffset(Delta * DeltaSeconds);
		}
	}
}

void AMMOPlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
}

void AMMOPlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}

void AMMOPlayerController::OnSelectPressed()
{
	if (GetMousePosition(CurrentMouseLocation.X, CurrentMouseLocation.Y))
	{
		bSelecting = true;
		MousePressedLocation = CurrentMouseLocation;
	}
}

void AMMOPlayerController::OnSelectReleased()
{
	if (!bSelecting)
		return;

	bSelecting = false;
}
