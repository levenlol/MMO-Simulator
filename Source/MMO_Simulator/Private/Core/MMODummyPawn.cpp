// Fill out your copyright notice in the Description page of Project Settings. 

#include "Core/MMODummyPawn.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Core/MMOPlayerController.h"
#include "Characters/MMOBaseHero.h"
#include "Camera/MMOPlayerCameraManager.h"
#include "Engine/World.h"
#include "Utils/MMOGameplayUtils.h"

AMMODummyPawn::AMMODummyPawn()
{
	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(RootComponent);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AMMODummyPawn::BeginPlay()
{
	Super::BeginPlay();

	CameraManager = AMMOPlayerCameraManager::GetPlayerCameraManager(this, 0);
	CurrentCameraRotationSpeed = 0.f;
}

void AMMODummyPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	MoveCameraMouse(DeltaSeconds);
	HandleRotateCamera(DeltaSeconds);
}

void AMMODummyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(FName("ToggleCamera"), EInputEvent::IE_Pressed, this, &AMMODummyPawn::ToggleCameraLock);
	PlayerInputComponent->BindAction(FName("RotateCamera"), EInputEvent::IE_Pressed, this, &AMMODummyPawn::CameraRotate_Pressed);
	PlayerInputComponent->BindAction(FName("RotateCamera"), EInputEvent::IE_Released, this, &AMMODummyPawn::CameraRotate_Released);

	PlayerInputComponent->BindAxis(FName("CameraUp"), this, &AMMODummyPawn::MoveCameraUp);
	PlayerInputComponent->BindAxis(FName("CameraRight"), this, &AMMODummyPawn::MoveCameraRight);
	PlayerInputComponent->BindAxis(FName("CameraZoom"), this, &AMMODummyPawn::CameraZoom);
	PlayerInputComponent->BindAxis(FName("RotateCameraAxis"), this, &AMMODummyPawn::CameraRotate_Axis);
}

void AMMODummyPawn::ToggleCameraLock()
{
	bCameraLocked = !bCameraLocked;
}

void AMMODummyPawn::HandleCameraLocked(const TArray<AMMOBaseHero*>& Heroes)
{
	if (Heroes.Num() > 0)
	{
		const FVector MiddlePoint = UMMOGameplayUtils::ComputeHeroesSelectionMiddlePoint(Heroes);
		SetActorLocation(MiddlePoint);
	}
}

void AMMODummyPawn::HandleFreeCamera(AMMOPlayerController* PlayerController, float DeltaSeconds)
{
	int32 SizeX, SizeY;
	PlayerController->GetViewportSize(SizeX, SizeY);

	float LocationX, LocationY;
	if (PlayerController->GetMousePosition(LocationX, LocationY))
	{
		const bool MoveUp = LocationY < BorderSize;
		const bool MoveDown = LocationY > (SizeY - BorderSize);
		const bool MoveLeft = LocationX < BorderSize;
		const bool MoveRight = LocationX > (SizeX - BorderSize);

		const float SpeedY = MoveUp ? CameraSpeed : MoveDown ? -CameraSpeed : 0.f;
		const float SpeedX = MoveRight ? CameraSpeed : MoveLeft ? -CameraSpeed : 0.f;

		const FVector Delta = GetActorForwardVector() * SpeedY + GetActorRightVector() * SpeedX;
		AddActorWorldOffset(Delta * DeltaSeconds);
	}
}

void AMMODummyPawn::CameraRotate_Pressed()
{
	AMMOPlayerController* PlayerController = Cast<AMMOPlayerController>(GetController());
	if (PlayerController && PlayerController->GetMousePosition(LastMousePosition.X, LastMousePosition.Y))
	{
		bRotatingCamera = true;
	}
}

void AMMODummyPawn::CameraRotate_Released()
{
	bRotatingCamera = false;
}

void AMMODummyPawn::CameraRotate_Axis(float AxisValue)
{
	RotateCameraYaw(AxisValue);
}

void AMMODummyPawn::RotateCameraYaw(float AxisValue)
{
	if (AxisValue == 0.f)
	{
		CurrentCameraRotationSpeed = 0.f;
		return;
	}

	const float DeltaSeconds = GetWorld()->GetDeltaSeconds();
	FMath::ExponentialSmoothingApprox(CurrentCameraRotationSpeed, CameraRotationSpeed, DeltaSeconds, CameraRotationSmoothSpeed);

	const float Delta = AxisValue * CurrentCameraRotationSpeed;
	AddActorWorldRotation(FRotator(0.f, Delta, 0.f));
}

void AMMODummyPawn::MoveCameraMouse(float DeltaSeconds)
{
	AMMOPlayerController* PlayerController = Cast<AMMOPlayerController>(GetController());

	if (!PlayerController)
	{
		return;
	}

	if (bCameraLocked)
	{
		if (!PlayerController->IsSelecting())
		{
			const TArray<AMMOBaseHero*>& Heroes = PlayerController->GetSelectedHeroes();
			HandleCameraLocked(Heroes);
		}
	}
	else
	{
		HandleFreeCamera(PlayerController, DeltaSeconds);
	}
}

void AMMODummyPawn::MoveCameraUp(float AxisValue)
{
	if (!bCameraLocked)
	{
		const FVector Delta = GetActorForwardVector() * CameraSpeed * AxisValue;
		AddActorWorldOffset(Delta * GetWorld()->DeltaTimeSeconds);
	}
}

void AMMODummyPawn::MoveCameraRight(float AxisValue)
{
	if (!bCameraLocked)
	{
		const FVector Delta = GetActorRightVector() * CameraSpeed * AxisValue;
		AddActorWorldOffset(Delta * GetWorld()->DeltaTimeSeconds);
	}
}

void AMMODummyPawn::HandleRotateCamera(float DeltaSeconds)
{
	AMMOPlayerController* PlayerController = Cast<AMMOPlayerController>(GetController());

	if (!bRotatingCamera || !PlayerController)
		return;

	const float PrevMouseX = LastMousePosition.X;
	if (PlayerController->GetMousePosition(LastMousePosition.X, LastMousePosition.Y))
	{
		const float Delta = (LastMousePosition.X - PrevMouseX);
		RotateCameraYaw(Delta * MouseCameraRotationMultiplier);
	}
}

void AMMODummyPawn::CameraZoom(float AxisValue)
{
	if (AxisValue != 0.f && CameraManager)
	{
		CameraManager->CameraZoom(AxisValue);
	}
}
