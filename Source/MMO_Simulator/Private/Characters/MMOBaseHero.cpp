// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/MMOBaseHero.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/DecalComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "CombatSystem/MMOCombatSystem.h"

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
	SelectionDecal->DecalSize = FVector(16.0, 32.0, 32.0);
	SelectionDecal->SetRelativeRotation(FRotator(90.0, 0.0, 0.0).Quaternion());

	SkillPreviewDecal = CreateDefaultSubobject<UDecalComponent>("SkillPreviewDecal");
	SkillPreviewDecal->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalPreviewAsset(TEXT("Material'/Game/Materials/Hud/M_Spell_Circle_Decal.M_Spell_Circle_Decal'"));
	if (DecalPreviewAsset.Succeeded())
	{
		SkillPreviewDecal->SetDecalMaterial(DecalPreviewAsset.Object);
	}
	SkillPreviewDecal->DecalSize = FVector(64., 100., 100.);
	SkillPreviewDecal->SetRelativeRotation(FRotator(90.0, 0.0, 0.0).Quaternion());
	SkillPreviewDecal->SetRelativeLocation(FVector::UpVector * -50.f);
	
	HideSkillRange();
}

void AMMOBaseHero::ShowSkillRange(const int32 Index)
{
	if (CanShowSkillPreview())
	{
		SkillPreviewDecal->SetHiddenInGame(false);

		const double Range = (double) CombatSystem->GetSkillRange(Index);
		if (Range > 0.)
		{
			SkillPreviewDecal->DecalSize = FVector(64., Range, Range);
		}
	}
}

void AMMOBaseHero::HideSkillRange()
{
	SkillPreviewDecal->SetHiddenInGame(true);
}

bool AMMOBaseHero::CanShowSkillPreview() const
{
	return !HasTag(FMMOStatusTags::Get().StunnedTag);
}

void AMMOBaseHero::OnSelected_Implementation()
{
	SelectionDecal->SetHiddenInGame(false);
}

void AMMOBaseHero::OnDeselected_Implementation()
{
	SelectionDecal->SetHiddenInGame(true);
	HideSkillRange();
}
