// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Core/MMODummyPawn.h"

#include "MMOPlayerController.generated.h"

class AMMOBaseHero;
class UMMOFormationManager;
class AMMOBaseEnemy;

UCLASS()
class MMO_SIMULATOR_API AMMOPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AMMOPlayerController();

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	UMMOFormationManager* FormationManager;

	UPROPERTY(EditAnywhere, Category = Combat)
	TEnumAsByte<ECollisionChannel> EnemyCollionChannel;

	UFUNCTION(BlueprintPure, Category = Default)
	FORCEINLINE AMMODummyPawn* GetDummyPawn() const { return Cast<AMMODummyPawn>(GetPawn()); }

	UFUNCTION(BlueprintPure, Category = Selection)
	FORCEINLINE bool IsSelecting() const { return bSelecting; }

	UFUNCTION(BlueprintPure, Category = Selection)
	FORCEINLINE bool IsMovingUnits() const { return bMovingUnits; }

	UFUNCTION(BlueprintPure, Category = Selection)
	const FVector2D& GetCurrentMousePosition() const { return CurrentMouseLocation; }

	UFUNCTION(BlueprintPure, Category = Selection)
	const FVector2D& GetMouseClickLocation() const { return MousePressedLocation; }

	UFUNCTION(BlueprintPure, Category = Selection)
	bool HasValidMousePositions() const { return bHasValidMousePosition;  }

	UFUNCTION(BlueprintCallable, Category = Selection)
	void SetSelectedHeroes(const TArray<AMMOBaseHero*>& InHeroes);

	FORCEINLINE const TArray<AMMOBaseHero*>& GetSelectedHeroes() const { return SelectedHeroes; }


protected:
	// Current selected heroes.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TArray<AMMOBaseHero*> SelectedHeroes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	bool bSelecting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	bool bMovingUnits = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	FVector2D MousePressedLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	FVector2D CurrentMouseLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	FVector MousePressedTerrainLocation;

	// Deproject mouse to terrain
	bool DeprojectMouseToTerrain(FVector& OutLocation, FVector& OutTerrainNormal) const;

private:
	bool bHasValidMousePosition = false;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface

	/** Navigate player to the given world location. */
	void SetNewMoveDestination();

	/** Input handlers for SetDestination action. */
	void MoveOrAttackPressed();
	void MoveOrAttackReleased();

	/** Input handlers for Select action. */
	void OnSelectPressed();
	void OnSelectReleased();
	AMMOBaseEnemy* GetEnemyUnderMouse() const;

	void MoveHeroTo(AController* InController, AActor* Target);
	void MoveHeroTo(AController* InController, const FVector& Location);
};
