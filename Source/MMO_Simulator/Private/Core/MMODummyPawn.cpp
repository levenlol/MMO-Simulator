// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MMODummyPawn.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
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
	CameraBoom->bDoCollisionTest = true;

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a decal in the world to show the cursor's location
	CursorToWorldDecal = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorldDecal->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/Materials/Hud/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		CursorToWorldDecal->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	CursorToWorldDecal->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorldDecal->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

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
}

void AMMODummyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMMODummyPawn::SetCursorLocationAndRotation(const FVector& WorldLocation, const FRotator& WorldRotator)
{
	if (CursorToWorldDecal != nullptr)
	{
		CursorToWorldDecal->SetWorldLocation(WorldLocation);
		CursorToWorldDecal->SetWorldRotation(WorldRotator);
	}
}
