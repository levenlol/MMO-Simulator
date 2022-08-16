// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MMODummyPawn.generated.h"

class UCameraComponent;
class AMMOPlayerCameraManager;
class UInputComponent;
class AMMOBaseHero;
class AMMOPlayerController;

UCLASS()
class MMO_SIMULATOR_API AMMODummyPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMMODummyPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	/** Toggle camera lock: switch between free camera movement and lock into target. */
	UFUNCTION(BlueprintCallable, Category = Camera)
	void ToggleCameraLock();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }

private:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = Debug, meta = (AllowPrivateAccess = "true"))
	bool bDebug = false;
#endif

	UPROPERTY()
	AMMOPlayerCameraManager* CameraManager = nullptr;

	// Camera Movement Speed when isn't locked.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float CameraSpeed = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float CameraRotationSpeed = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	bool bCameraLocked = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	int32 BorderSize = 5;

	// True if MiddleMouseButton is pressed.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	bool bRotatingCamera = false;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	float CurrentCameraRotationSpeed = 0.f;

	UPROPERTY(EditAnywhere, Category = Camera)
	float CameraRotationSmoothSpeed = 0.25f;

	UPROPERTY(EditAnywhere, Category = Camera)
	float MouseCameraRotationMultiplier = 1.f;
	
	UPROPERTY(VisibleAnywhere, Category = Camera)
	FVector2D LastMousePosition;

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* TopDownCameraComponent;

	/** Handle camera movements */
	void MoveCameraMouse(float DeltaSeconds);
	void MoveCameraUp(float AxisValue);
	void MoveCameraRight(float AxisValue);

	void HandleRotateCamera(float DeltaSeconds);

	void CameraZoom(float AxisValue);

	void HandleCameraLocked(const TArray<AMMOBaseHero*>& Heroes);
	void HandleFreeCamera(AMMOPlayerController* PlayerController, float DeltaSeconds);

	void CameraRotate_Pressed();
	void CameraRotate_Released();
	void CameraRotate_Axis(float AxisValue);

	void RotateCameraYaw(float AxisValue);
};
