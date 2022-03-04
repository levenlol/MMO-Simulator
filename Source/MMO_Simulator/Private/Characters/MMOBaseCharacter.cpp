// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/MMOBaseCharacter.h"
#include "Weapons/MMOBaseWeapon.h"
#include "GameplayTagsManager.h"
#include "TimerManager.h"
#include "CombatSystem/MMOCombatSystem.h"
#include "Core/MMOGameState.h"

// Sets default values
AMMOBaseCharacter::AMMOBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CombatSystem = CreateDefaultSubobject<UMMOCombatSystem>(TEXT("CombatSystem"));
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void AMMOBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(RecuperateTimerHandle, this, &AMMOBaseCharacter::OnRecuperate, static_cast<float>(CharacterInfo.Stats.RecuperateEverySeconds), true, -1.f);
}

void AMMOBaseCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(RecuperateTimerHandle);
}

void AMMOBaseCharacter::Die_Implementation()
{
	if (AMMOGameState* GameState = AMMOGameState::GetMMOGameState(this))
	{
		GameState->NotifyDeath(this);
	}
}

void AMMOBaseCharacter::OnRecuperate()
{
	CharacterInfo.Stats.Recuperate(CharacterInfo.Stats.RecuperateEverySeconds);
}

void AMMOBaseCharacter::DamageTake_Implementation(FMMODamage InDamage)
{
	CharacterInfo.Stats.Health = FMath::Clamp(CharacterInfo.Stats.Health - InDamage.Damage, 0, CharacterInfo.Stats.MaxHealth);
	OnDamageTaken.Broadcast(this, InDamage);

	if (CharacterInfo.Stats.Health <= 0 && bAlive)
	{
		bAlive = false;
		Die();
	}
}

FMMOWeaponTypeCouple AMMOBaseCharacter::GetCurrentEquippedWeaponsType() const
{
	FMMOWeaponTypeCouple CurrentEquippedWeapons(EMMOWeaponType::Unarmed, EMMOWeaponType::Unarmed);

	if (MainHandWeapon)
	{
		CurrentEquippedWeapons.MainWeaponType = MainHandWeapon->WeaponType;
	}

	if (OffHandWeapon)
	{
		CurrentEquippedWeapons.OffWeaponType = OffHandWeapon->WeaponType;
	}

	return CurrentEquippedWeapons;
}

float AMMOBaseCharacter::GetWeaponRange() const
{
	float WeaponRange = 0.f;
	if (MainHandWeapon)
	{
		WeaponRange = MainHandWeapon->Stats.WeaponRange;
	}

	if (OffHandWeapon)
	{
		WeaponRange = FMath::Min(WeaponRange, OffHandWeapon->Stats.WeaponRange);
	}

	return WeaponRange;
}

bool AMMOBaseCharacter::HasEquippedAnyWeapons() const
{
	return !!MainHandWeapon;
}

bool AMMOBaseCharacter::TryEquipWeapon(TSubclassOf<AMMOBaseWeapon> InWeaponClass, bool bMainHand)
{
	if (!InWeaponClass)
		return false;

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.Instigator = this;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AMMOBaseWeapon* Weapon = GetWorld()->SpawnActor<AMMOBaseWeapon>(InWeaponClass, SpawnParameters);

	if (!Weapon)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn Weapon: %s"), *InWeaponClass->GetName());
		return false;
	}

	const bool bIs2H = UMMOGameplayUtils::Is2HWeapon(Weapon->WeaponType);
	if (MainHandWeapon && (bMainHand || bIs2H))
	{
		// TODO unequip main hand weapon
		MainHandWeapon->Destroy();
		MainHandWeapon = nullptr;
	}

	if (OffHandWeapon && (!bMainHand || bIs2H))
	{
		// TODO unequip off hand weapon
		OffHandWeapon->Destroy();
		OffHandWeapon = nullptr;
	}

	bMainHand = bMainHand || bIs2H; // 2h weapon should be put in the main hand

	AMMOBaseWeapon* OldWeapon = nullptr;

	// assign and attach new weapon
	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
	if (bMainHand || bIs2H)
	{
		OldWeapon = MainHandWeapon;
		MainHandWeapon = Weapon;
		Weapon->AttachToComponent(GetMesh(), AttachRules, MainHandSocket);
	}
	else
	{
		OldWeapon = OffHandWeapon;
		OffHandWeapon = Weapon;
		Weapon->AttachToComponent(GetMesh(), AttachRules, OffHandSocket);
	}

	OnEquipWeapon.Broadcast(this, Weapon, OldWeapon);
	return true;
}

void AMMOBaseCharacter::GiveTag(const FGameplayTag& InTag)
{
	StatusTags.AddTag(InTag);
}

void AMMOBaseCharacter::RemoveTag(const FGameplayTag& InTag)
{
	StatusTags.RemoveTag(InTag);
}

bool AMMOBaseCharacter::HasTag(const FGameplayTag& InTag) const
{
	return StatusTags.HasTag(InTag);
}

// Called every frame
void AMMOBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
