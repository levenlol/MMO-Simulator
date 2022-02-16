// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Core/MMODummyPawn.h"

#include "MMOPlayerController.generated.h"

class AMMOBaseHero;

UCLASS()
class MMO_SIMULATOR_API AMMOPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AMMOPlayerController();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category = Default)
	FORCEINLINE AMMODummyPawn* GetDummyPawn() const { return Cast<AMMODummyPawn>(GetPawn()); }

	UFUNCTION(BlueprintPure, Category = Selection)
	FORCEINLINE bool IsSelecting() const { return bSelecting; }

	UFUNCTION(BlueprintPure, Category = Selection)
	const FVector2D& GetCurrentMousePosition() const { return CurrentMouseLocation; }

	UFUNCTION(BlueprintPure, Category = Selection)
	const FVector2D& GetMouseClickLocation() const { return MousePressedLocation; }

	UFUNCTION(BlueprintPure, Category = Selection)
	bool HasValidMousePositions() const { return bHasValidMousePosition;  }

	UFUNCTION(BlueprintCallable, Category = Selection)
	void SetSelectedHeroes(const TArray<AMMOBaseHero*>& InHeroes);

	UFUNCTION(BlueprintPure, Category = Selection)
	FORCEINLINE TArray<AMMOBaseHero*> GetSelectedHeroes() const { return SelectedHeroes; }

protected:
	// Camera Movement Speed when isn't locked.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	float CameraSpeed = 1000.f;

	// Current selected heroes.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TArray<AMMOBaseHero*> SelectedHeroes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	bool bCameraLocked = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	bool bSelecting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	FVector2D MousePressedLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	FVector2D CurrentMouseLocation;

	// Deproject mouse to terrain
	bool DeprojectMouseToTerrain(FVector& OutLocation, FVector& OutTerrainNormal) const;

private:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;
	uint32 bHasValidMousePosition : 1;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface

	/** Navigate player to the current mouse cursor location. */
	void MoveToMouseCursor();

	/** Navigate player to the given world location. */
	void SetNewMoveDestination(const FVector DestLocation);

	/** Toggle camera lock: switch between free camera movement and lock into target. */
	void ToggleCameraLock();

	/** Handle camera movements */
	void MoveCamera(float DeltaSeconds);

	/** Input handlers for SetDestination action. */
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();

	/** Input handlers for Select action. */
	void OnSelectPressed();
	void OnSelectReleased();
};
