// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/MMOBaseCharacter.h"
#include "Items/MMOBaseWeapon.h"
#include "GameplayTagsManager.h"
#include "TimerManager.h"
#include "CombatSystem/MMOCombatSystem.h"
#include "Core/MMOGameState.h"
#include "Data/MMOStatsManager.h"
#include "CombatSystem/MMODebuffManager.h"
#include "CombatSystem/MMOBuffManager.h"
#include "Animation/AnimationAsset.h"
#include "Components/CapsuleComponent.h"
#include "Utils/MMOGameplayUtils.h"


FMMOStatusTags FMMOStatusTags::StatusTags;

// Sets default values
AMMOBaseCharacter::AMMOBaseCharacter()
	: Super()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CombatSystem = CreateDefaultSubobject<UMMOCombatSystem>(TEXT("CombatSystem"));
	StatsManager = CreateDefaultSubobject<UMMOStatsManager>(TEXT("StatsManager"));
	DebuffManager = CreateDefaultSubobject<UMMODebuffManager>(TEXT("DebuffManager"));
	BuffManager = CreateDefaultSubobject<UMMOBuffManager>(TEXT("BuffManager"));
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
}

void AMMOBaseCharacter::Stun_Implementation(float InDuration)
{
	if (InDuration <= 0.f)
		return;

	GiveTag(FMMOStatusTags::Get().StunnedTag);
	StunTimerRemaining = FMath::Max(StunTimerRemaining, InDuration);
}

void AMMOBaseCharacter::EquipArmor(EMMOArmorSlotType Slot, const FMMOItemStats& InItem)
{
	FMMOItemStats& FindSlot = EquippedArmor.FindOrAdd(Slot);
	FindSlot = InItem;

	OnArmorEquipped.Broadcast(this, Slot, InItem);
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

void AMMOBaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AMMOBaseCharacter::Die_Implementation()
{
	if (AMMOGameState* GameState = AMMOGameState::GetMMOGameState(this))
	{
		if (AController* CurrentController = GetController())
		{
			CurrentController->UnPossess();
		}

		if (DeathAnimationAsset)
		{
			GetMesh()->PlayAnimation(DeathAnimationAsset, false);
		}

		GetCapsuleComponent()->SetCollisionProfileName(TEXT("NoCollision"));

		bAlive = false;
		GameState->NotifyDeath(this);
	}
}

void AMMOBaseCharacter::OnRecuperate()
{
	CharacterInfo.Stats.Recuperate(CharacterInfo.Stats.RecuperateEverySeconds);
}

void AMMOBaseCharacter::DamageTake(FMMODamage InDamage)
{
	InDamage = BuffManager->ProcessDamageTaken(InDamage); // much likely it means resistances
	InDamage = DebuffManager->ProcessDamageTaken(InDamage); // much likely means vulnerabilities

	// handle physical damage. Physical damages can be dodged/blocked etc
	int32 DamageToApply = InDamage.Damage;
	
	// Damage > 0 indicates that is a damage. Damage < 0 is a healing
	if (DamageToApply > 0)
	{	
		// Damage reduction percentage.
		const float DamageReduction = UMMOGameplayUtils::GetDamageReduction(this, InDamage.DamageDealer, InDamage.DamageType);

		DamageToApply = FMath::Max(0, DamageToApply - DamageToApply * DamageReduction);
	}

	// actual damage implementation.
	CharacterInfo.Stats.Health = FMath::Clamp(CharacterInfo.Stats.Health - DamageToApply, 0, CharacterInfo.Stats.MaxHealth);

	InDamage.Damage = DamageToApply;
	OnDamageTaken.Broadcast(this, InDamage);

	if (CharacterInfo.Stats.Health <= 0 && bAlive)
	{
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

void AMMOBaseCharacter::InitializeCharacter(const FMMOCharacter& InCharacter)
{
	CharacterInfo = InCharacter;

	OnCharacterInitialized.Broadcast(this);
}

void AMMOBaseCharacter::DoDamage(FMMODamage InDamage, AMMOBaseCharacter* Target)
{
	if (Target)
	{
		Target->DamageTake(InDamage);
	}
}

void AMMOBaseCharacter::OnLevelUp_Implementation()
{
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
	if (InTag == FMMOStatusTags::Get().StunnedTag)
	{
		OnCharacterStunned.Broadcast(this);
	}

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

	if (HasTag(FMMOStatusTags::Get().StunnedTag))
	{
		StunTimerRemaining -= DeltaTime;
		if (StunTimerRemaining < 0.f)
		{
			StunTimerRemaining = 0.f;
			RemoveTag(FMMOStatusTags::Get().StunnedTag);
		}
	}
}
