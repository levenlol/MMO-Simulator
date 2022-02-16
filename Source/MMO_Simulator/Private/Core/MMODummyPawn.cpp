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

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 1200.f;
	CameraBoom->SetRelativeRotation(FRotator(-70.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false;

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
}

void AMMODummyPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	MoveCameraMouse(DeltaSeconds);
}

void AMMODummyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(FName("ToggleCamera"), EInputEvent::IE_Pressed, this, &AMMODummyPawn::ToggleCameraLock);

	PlayerInputComponent->BindAxis(FName("CameraUp"), this, &AMMODummyPawn::MoveCameraUp);
	PlayerInputComponent->BindAxis(FName("CameraRight"), this, &AMMODummyPawn::MoveCameraRight);
}

void AMMODummyPawn::ToggleCameraLock()
{
	bCameraLocked = !bCameraLocked;
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
			if (Heroes.Num() > 0)
			{
				float X = 0.f;
				float Y = 0.f;
				float Z = 0.f;

				// fly average
				for (int32 i = 0; i < Heroes.Num(); i++)
				{
					const FVector& Location = Heroes[i]->GetActorLocation();
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
	}
	else
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

			const FVector Delta = FVector::ForwardVector * SpeedY + FVector::RightVector * SpeedX;
			AddActorWorldOffset(Delta * DeltaSeconds);
		}
	}
}

void AMMODummyPawn::MoveCameraUp(float AxisValue)
{
	const FVector Delta = FVector::ForwardVector * CameraSpeed * AxisValue;
	AddActorWorldOffset(Delta * GetWorld()->DeltaTimeSeconds);
}

void AMMODummyPawn::MoveCameraRight(float AxisValue)
{
	const FVector Delta = FVector::RightVector * CameraSpeed * AxisValue;
	AddActorWorldOffset(Delta * GetWorld()->DeltaTimeSeconds);
}
