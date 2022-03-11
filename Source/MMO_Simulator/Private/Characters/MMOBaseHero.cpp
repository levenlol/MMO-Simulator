// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/MMOBaseHero.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/DecalComponent.h"
#include "UObject/ConstructorHelpers.h"

AMMOBaseHero::AMMOBaseHero()
	: Super()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName(FName("HeroPawn"));

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a decal in the world to show the cursor's location
	SelectionDecal = CreateDefaultSubobject<UDecalComponent>("SelectionDecal");
	SelectionDecal->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/Materials/Hud/M_Selected_Decal.M_Selected_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		SelectionDecal->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	SelectionDecal->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	SelectionDecal->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());
}

void AMMOBaseHero::OnSelected_Implementation()
{
	SelectionDecal->SetHiddenInGame(false);
}

void AMMOBaseHero::OnDeselected_Implementation()
{
	SelectionDecal->SetHiddenInGame(true);
}
