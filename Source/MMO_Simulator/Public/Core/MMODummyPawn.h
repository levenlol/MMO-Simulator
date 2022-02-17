// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MMODummyPawn.generated.h"

class UCameraComponent;
class USpringArmComponent;
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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

private:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = Debug, meta = (AllowPrivateAccess = "true"))
	bool bDebug = false;
#endif

	// Camera Movement Speed when isn't locked.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float CameraSpeed = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float CameraRotationSpeed = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	FVector2D CameraZoomRange = FVector2D(800.f, 2000.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float CameraZoomSpeed = 200.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	bool bCameraLocked = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	bool bRotatingCamera = false;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	float TargetZoom;
	
	UPROPERTY(VisibleAnywhere, Category = Camera)
	FVector2D LastMousePosition;

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Handle camera movements */
	void MoveCameraMouse(float DeltaSeconds);
	void MoveCameraUp(float AxisValue);
	void MoveCameraRight(float AxisValue);

	void HandleRotateCamera(float DeltaSeconds);

	void CameraZoom(float AxisValue);

	/** Toggle camera lock: switch between free camera movement and lock into target. */
	void ToggleCameraLock();

	void HandleCameraLocked(const TArray<AMMOBaseHero*>& Heroes);
	void HandleFreeCamera(AMMOPlayerController* PlayerController, float DeltaSeconds);

	void HandleCameraZoom(float DeltaSeconds);

	void CameraRotate_Pressed();
	void CameraRotate_Released();
};
