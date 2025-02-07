// Fill out your copyright notice in the Description page of Project Settings. 

#include "Core/MMODummyPawn.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Core/MMOPlayerController.h"
#include "Characters/MMOBaseHero.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/World.h"

AMMODummyPawn::AMMODummyPawn()
{
	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(false); 
	CameraBoom->TargetArmLength = 2300.f;
	CameraBoom->SetRelativeRotation(FRotator(-70.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false;

	TargetZoom = CameraBoom->TargetArmLength;

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AMMODummyPawn::BeginPlay()
{
	Super::BeginPlay();

	TargetZoom = CameraBoom->TargetArmLength;
}

void AMMODummyPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	HandleCameraZoom(DeltaSeconds);
	MoveCameraMouse(DeltaSeconds);
	HandleRotateCamera(DeltaSeconds);

#if WITH_EDITORONLY_DATA
	if (bDebug)
	{
		GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Blue, FString("Current: ") + FString::SanitizeFloat(CameraBoom->TargetArmLength) + FString(" - Target: ") + FString::SanitizeFloat(TargetZoom) + FString(" - Delta: ") + FString::SanitizeFloat(TargetZoom - CameraBoom->TargetArmLength));
		
		DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 200.f, FColor::Red, false, -1.f, 0, 10.f);
		DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + GetActorRightVector() * 200.f, FColor::Green, false, -1.f, 0, 10.f);
		DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + GetActorUpVector() * 200.f, FColor::Blue, false, -1.f, 0, 10.f);
	}
#endif
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
		float X = 0.f;
		float Y = 0.f;
		float Z = 0.f;

		// fly average
		for (int32 i = 0; i < Heroes.Num(); i++)
		{
			const FVector Location = Heroes[i]->GetActorLocation();
			const float n = (i + 1.f);
			const float a = 1 / n;
			const float b = 1 - a;

			X = a * Location.X + b * X;
			Y = a * Location.Y + b * Y;
			Z = a * Location.Z + b * Z;
		}

		SetActorLocation(FVector(X, Y, Z));
	}
}

void AMMODummyPawn::HandleFreeCamera(AMMOPlayerController* PlayerController, float DeltaSeconds)
{
	constexpr int32 BorderSize = 50;

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

void AMMODummyPawn::HandleCameraZoom(float DeltaSeconds)
{
	if (CameraBoom->TargetArmLength != TargetZoom)
	{
		if (FMath::IsNearlyEqual(CameraBoom->TargetArmLength, TargetZoom, 0.1f))
		{
			CameraBoom->TargetArmLength = TargetZoom;
		}
		else
		{
			// smooth interpolation
			const float NewArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, TargetZoom, DeltaSeconds, 3.f);
			CameraBoom->TargetArmLength = NewArmLength;
		}
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
	RotateCameraYaw(AxisValue * CameraSpeed * GetWorld()->GetDeltaSeconds());
}

void AMMODummyPawn::RotateCameraYaw(float Delta)
{
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
		const float Delta = (LastMousePosition.X - PrevMouseX) * DeltaSeconds * CameraRotationSpeed;
		RotateCameraYaw(Delta);
	}
}

void AMMODummyPawn::CameraZoom(float AxisValue)
{
	if (AxisValue != 0.f)
	{
		const float CurrentLength = CameraBoom->TargetArmLength;

		const float NewLength = FMath::Clamp(CurrentLength + AxisValue * CameraZoomSpeed, CameraZoomRange.X, CameraZoomRange.Y);
		TargetZoom = NewLength;
	}
}
