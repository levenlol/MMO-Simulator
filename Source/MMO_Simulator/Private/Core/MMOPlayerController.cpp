// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MMOPlayerController.h"
#include "Engine/World.h"
#include "Characters/MMOBaseHero.h"
#include "Core/MMOBaseHUD.h"
#include "AI/MMOFormationManager.h"
#include "Characters/MMOBaseEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AI/MMOAIController.h"

AMMOPlayerController::AMMOPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;

	FormationManager = CreateDefaultSubobject<UMMOFormationManager>(TEXT("FormationManager"));
	EnemyCollionChannel = ECollisionChannel::ECC_GameTraceChannel1; // Enemy Channel.
	CharacterCollionChannel = ECollisionChannel::ECC_GameTraceChannel2; // Character Channel
	HeroCollisionChannel = ECollisionChannel::ECC_GameTraceChannel3; // Hero Channel
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

void AMMOPlayerController::TogglePause()
{
	const float TimeDilation = UGameplayStatics::GetGlobalTimeDilation(this);
	UGameplayStatics::SetGlobalTimeDilation(this, FMath::IsNearlyZero(TimeDilation, 0.001f) ? 1.f : 0.f);
}

bool AMMOPlayerController::DeprojectMouseToTerrain(FVector& OutLocation, FVector& OutTerrainNormal) const
{
	FVector MouseLocation, Direction;
	if (DeprojectMousePositionToWorld(MouseLocation, Direction))
	{
		FHitResult HitResult;
		if(GetWorld()->LineTraceSingleByChannel(HitResult, MouseLocation, MouseLocation + Direction * 3500.f, ECollisionChannel::ECC_WorldStatic))
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
			const TArray<FVector> Points = FormationManager->ComputeFormation(SelectedHeroes.Num(), MousePressedTerrainLocation, Location);
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

	InputComponent->BindAction("Pause", IE_Released, this, &AMMOPlayerController::TogglePause);
	
	InputComponent->BindAction("Skill_1", IE_Released, this, &AMMOPlayerController::TryCastSkill<0>);
	InputComponent->BindAction("Skill_2", IE_Released, this, &AMMOPlayerController::TryCastSkill<1>);
	InputComponent->BindAction("Skill_3", IE_Released, this, &AMMOPlayerController::TryCastSkill<2>);

}

void AMMOPlayerController::SetNewMoveDestination()
{
	if (SelectedHeroes.Num() <= 0)
		return;

	FVector Location, ImpactNormal;
	if (DeprojectMouseToTerrain(Location, ImpactNormal))
	{
		const TArray<FVector> Points = FormationManager->ComputeFormation(SelectedHeroes.Num(), MousePressedTerrainLocation, Location);

		for (int32 i = 0; i < SelectedHeroes.Num(); i++)
		{
			AMMOBaseHero* CurrentHero = SelectedHeroes[i];
			const FVector& DestLocation = Points[i];

			MoveHeroTo(CurrentHero->GetController(), DestLocation);
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
				MoveHeroTo(Hero->GetController(), Enemy);
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
	return Cast<AMMOBaseEnemy>(GetCharacterUnderMouse_Internal(EnemyCollionChannel));
}

AMMOBaseCharacter* AMMOPlayerController::GetCharacterUnderMouse() const
{
	return GetCharacterUnderMouse_Internal(CharacterCollionChannel);
}

AMMOBaseHero* AMMOPlayerController::GetHeroUnderMouse() const
{
	return Cast<AMMOBaseHero>(GetCharacterUnderMouse_Internal(HeroCollisionChannel));
}

AMMOBaseCharacter* AMMOPlayerController::GetCharacterUnderMouse_Internal(ECollisionChannel CollisionChannel) const
{
	FVector MouseLocation, Direction;
	if (DeprojectMousePositionToWorld(MouseLocation, Direction))
	{
		const FVector& StartLocation = MouseLocation;
		const FVector EndLocation = MouseLocation + Direction * 3500.f;

		FCollisionQueryParams Params;

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, CollisionChannel))
		{
			return Cast<AMMOBaseCharacter>(Hit.GetActor());
		}
	}

	return nullptr;
}

void AMMOPlayerController::MoveHeroTo(AController* InController, AActor* Target)
{
	if (AAIController* AIController = Cast<AAIController>(InController))
	{
		if (UBlackboardComponent* BlackBoard = AIController->GetBlackboardComponent())
		{
			BlackBoard->SetValueAsObject(FName("TargetActor"), Target); // set target actor, AI will move the Pawn to be in range with Target
			BlackBoard->SetValueAsObject(FName("SpellActorTarget"), Target); // Spells Target.
			BlackBoard->ClearValue(FName("TargetLocation")); // We will chase Target.
		}
	}
}

void AMMOPlayerController::MoveHeroTo(AController* InController, const FVector& Location)
{
	if (AAIController* AIController = Cast<AAIController>(InController))
	{
		if (UBlackboardComponent* BlackBoard = AIController->GetBlackboardComponent())
		{
			BlackBoard->SetValueAsVector(FName("TargetLocation"), Location); // Set Target location.
			BlackBoard->ClearValue(FName("TargetActor")); // Clear Target actor, we will not follow him anymore
			BlackBoard->ClearValue(FName("SpellActorTarget")); // Clear Spell target too. AI Will pick the best one.
		}
	}
}

void AMMOPlayerController::TryCastSkill(const int32 Index)
{
	AMMOBaseCharacter* Target = GetCharacterUnderMouse();
	FVector Location, Normal;
	DeprojectMouseToTerrain(Location, Normal);

	const TArray<AMMOBaseHero*>& Heroes = GetSelectedHeroes();
	for (AMMOBaseHero* Hero : Heroes)
	{
		// Skills are handled by AI.
		if (AMMOAIController* AIController = Cast<AMMOAIController>(Hero->GetController()))
		{
			AIController->RequestCastSpell(Target, Location, Index);
		}
	}
}
