// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MMOPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Engine/World.h"
#include "Characters/MMOBaseHero.h"
#include "Core/MMOBaseHUD.h"
#include "AI/MMOFormationManager.h"

AMMOPlayerController::AMMOPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;

	FormationManager = CreateDefaultSubobject<UMMOFormationManager>(TEXT("FormationManager"));
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
	if (IsInputKeyDown(EKeys::LeftControl))
	{
		// Add to selection
		for (AMMOBaseHero* CurrentHero : InHeroes)
		{
			if (!SelectedHeroes.Contains(CurrentHero))
			{
				SelectedHeroes.Add(CurrentHero);
				CurrentHero->OnSelected();
			}
		}
	}
	else
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

	bHasValidMousePosition = GetMousePosition(CurrentMouseLocation.X, CurrentMouseLocation.Y);

	if (IsMovingUnits())
	{
		FVector Location, ImpactNormal;
		if (DeprojectMouseToTerrain(Location, ImpactNormal))
		{
			const TArray<FVector> Points = FormationManager->ComputeFormation(SelectedHeroes, MousePressedTerrainLocation, Location);

		}
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

void AMMOPlayerController::SetNewMoveDestination()
{
	if (SelectedHeroes.Num() <= 0)
		return;

	FVector Location, ImpactNormal;
	if (DeprojectMouseToTerrain(Location, ImpactNormal))
	{
		const TArray<FVector> Points = FormationManager->ComputeFormation(SelectedHeroes, MousePressedTerrainLocation, Location);

		for (int32 i = 0; i < SelectedHeroes.Num(); i++)
		{
			AMMOBaseHero* CurrentHero = SelectedHeroes[i];
			const FVector& DestLocation = Points[i];

			UAIBlueprintHelperLibrary::SimpleMoveToLocation(CurrentHero->GetController(), DestLocation);

			//float const Distance = FVector::Dist(DestLocation, CurrentHero->GetActorLocation());
			//
			//// We need to issue move command only if far enough in order for walk animation to play correctly
			//if ((Distance > 50.0f))
			//{
			//	UAIBlueprintHelperLibrary::SimpleMoveToLocation(CurrentHero->GetController(), DestLocation);
			//}
		}
	}
}

void AMMOPlayerController::OnSetDestinationPressed()
{
	if (!IsSelecting() && SelectedHeroes.Num() > 0 && GetMousePosition(CurrentMouseLocation.X, CurrentMouseLocation.Y))
	{
		bMovingUnits = true;
		MousePressedLocation = CurrentMouseLocation;

		FVector Unusued;
		DeprojectMouseToTerrain(MousePressedTerrainLocation, Unusued);
	}
}

void AMMOPlayerController::OnSetDestinationReleased()
{
	if (!bMovingUnits)
		return;

	SetNewMoveDestination();
	bMovingUnits = false;
}

void AMMOPlayerController::OnSelectPressed()
{
	if (!IsMovingUnits() && GetMousePosition(CurrentMouseLocation.X, CurrentMouseLocation.Y))
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
