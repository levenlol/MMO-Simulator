// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MMOPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Engine/World.h"
#include "Characters/MMOBaseHero.h"
#include "Core/MMOBaseHUD.h"
#include "AI/MMOFormationManager.h"
#include "Characters/MMOBaseEnemy.h"

AMMOPlayerController::AMMOPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;

	FormationManager = CreateDefaultSubobject<UMMOFormationManager>(TEXT("FormationManager"));
	EnemyCollionChannel = ECollisionChannel::ECC_GameTraceChannel1; // Enemy Channel.
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
		if (GetWorld()->LineTraceSingleByObjectType(HitResult, MouseLocation, MouseLocation + Direction * 3500.f, Params))
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
	FormationManager->HideAllPreviews();

	// show previews
	if (IsMovingUnits())
	{
		FVector Location, ImpactNormal;
		if (DeprojectMouseToTerrain(Location, ImpactNormal))
		{
			const TArray<FVector> Points = FormationManager->ComputeFormation(SelectedHeroes, MousePressedTerrainLocation, Location);
			FormationManager->ShowPreview(Points);
		}
	}
}

void AMMOPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("MoveOrAttack", IE_Pressed, this, &AMMOPlayerController::MoveOrAttackPressed);
	InputComponent->BindAction("MoveOrAttack", IE_Released, this, &AMMOPlayerController::MoveOrAttackReleased);

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
		}
	}
}

void AMMOPlayerController::MoveOrAttackPressed()
{
	if (!IsSelecting() && SelectedHeroes.Num() > 0 && GetMousePosition(CurrentMouseLocation.X, CurrentMouseLocation.Y))
	{
		if (AMMOBaseEnemy* Enemy = GetEnemyUnderMouse())
		{
			for (AMMOBaseHero* Hero : SelectedHeroes)
			{
				UAIBlueprintHelperLibrary::SimpleMoveToActor(Hero->GetController(), Enemy);
			}
		}
		else
		{
			bMovingUnits = true;
			MousePressedLocation = CurrentMouseLocation;

			FVector Unusued;
			DeprojectMouseToTerrain(MousePressedTerrainLocation, Unusued);
		}
	}
}

void AMMOPlayerController::MoveOrAttackReleased()
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

AMMOBaseEnemy* AMMOPlayerController::GetEnemyUnderMouse() const
{
	FVector MouseLocation, Direction;
	if (DeprojectMousePositionToWorld(MouseLocation, Direction))
	{
		const FVector& StartLocation = MouseLocation;
		const FVector EndLocation = MouseLocation + Direction * 3500.f;

		FCollisionQueryParams Params;
		
		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, EnemyCollionChannel))
		{
			return Cast<AMMOBaseEnemy>(Hit.GetActor());
		}
	}

	return nullptr;
}
