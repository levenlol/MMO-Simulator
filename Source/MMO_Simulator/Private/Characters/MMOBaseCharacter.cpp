// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/MMOBaseCharacter.h"
#include "Weapons/MMOBaseWeapon.h"
#include "GameplayTagsManager.h"
#include "TimerManager.h"

// Sets default values
AMMOBaseCharacter::AMMOBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AttackTag = UGameplayTagsManager::Get().RequestGameplayTag(FName("Status.Action.Attack"));
	StunnedTag = UGameplayTagsManager::Get().RequestGameplayTag(FName("Status.Malus.Stunned"));
}

// Called when the game starts or when spawned
void AMMOBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(RecuperateTimerHandle, this, &AMMOBaseCharacter::OnRecuperate, static_cast<float>(Stats.RecuperateEverySeconds), true, -1.f);
}

void AMMOBaseCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(RecuperateTimerHandle);
}

FMMODamage AMMOBaseCharacter::ComputeAutoAttackDamage()
{
	FMMODamage Damage;

	if (MainHandWeapon)
	{
		const FMMOWeaponStats& WeaponStats = MainHandWeapon->Stats;
		Damage.Damage = FMath::RandRange(WeaponStats.Damage.X, WeaponStats.Damage.Y);
		Damage.bCrit = WeaponStats.CritChance >= FMath::RandRange(0.f, 1.f);
		Damage.DamageType = EMMODamageType::Physical;
		Damage.DamageDealer = this;
	}

	return Damage;
}

void AMMOBaseCharacter::OnRecuperate()
{
	Stats.Recuperate(Stats.RecuperateEverySeconds);
}

void AMMOBaseCharacter::DamageTake_Implementation(FMMODamage InDamage)
{
	Stats.Health = FMath::Clamp(Stats.Health - InDamage.Damage, 0, Stats.MaxHealth);
	OnDamageTaken.Broadcast(this, InDamage);
}

bool AMMOBaseCharacter::IsAttacking() const
{
	return StatusTags.HasTag(AttackTag);
}

bool AMMOBaseCharacter::IsStunned() const
{
	return StatusTags.HasTag(StunnedTag);
}

bool AMMOBaseCharacter::CanCharacterAttack() const
{
	const float CurrentTime = GetWorld()->GetTimeSeconds();

	const bool bIsStunned = IsStunned();
	return MainHandWeapon && !bIsStunned && (GetWorld()->GetTimeSeconds() - LastAttackTime) > MainHandWeapon->Stats.AttackSpeed;
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

	// assign and attach new weapon
	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
	if (bMainHand || bIs2H)
	{
		MainHandWeapon = Weapon;
		Weapon->AttachToComponent(GetMesh(), AttachRules, MainHandSocket);
	}
	else
	{
		OffHandWeapon = Weapon;
		Weapon->AttachToComponent(GetMesh(), AttachRules, OffHandSocket);
	}

	LastAttackTime = GetWorld()->GetTimeSeconds();

	return true;
}

bool AMMOBaseCharacter::TryAttack(AMMOBaseCharacter* Target)
{
	if (!CanAttackTarget(Target) || !MainHandWeapon)
	{
		StopAttack();
		return false;
	}

	// TODO this is wrong, should be start attack.

	FMMODamage Damage = ComputeAutoAttackDamage();
	Target->DamageTake(MoveTemp(Damage));

	OnStartAttack.Broadcast(this);
	return true;
}

void AMMOBaseCharacter::StopAttack()
{
	if (!IsAttacking())
	{
		return;
	}

	StatusTags.RemoveTag(AttackTag);
}

bool AMMOBaseCharacter::StartAttack(AMMOBaseCharacter* Target)
{
	if (!CanAttackTarget(Target) || !MainHandWeapon)
	{
		return false;
	}

	StatusTags.AddTag(AttackTag);

	// Damage Delegate
	FTimerHandle UnusuedHandle;
	FTimerDelegate AttackDelegate;

	TWeakObjectPtr<AMMOBaseCharacter> TargetWeak = Target;
	TWeakObjectPtr<AMMOBaseCharacter> ThisWeak = this;

	AttackDelegate.BindLambda([ThisWeak, TargetWeak] ()
	{
		if (!ThisWeak.IsValid())
		{
			return;
		}

		ThisWeak->TryAttack(TargetWeak.Get());
	});

	// Stop autoattack delegate
	FTimerHandle UnusuedHandle2;

	GetWorldTimerManager().SetTimer(UnusuedHandle, AttackDelegate, MainHandWeapon->Stats.AttackSpeed * 0.5f, false, MainHandWeapon->Stats.AttackSpeed * 0.5f);
	GetWorldTimerManager().SetTimer(UnusuedHandle2, this, &AMMOBaseCharacter::StopAttack, MainHandWeapon->Stats.AttackSpeed, false, MainHandWeapon->Stats.AttackSpeed);

	return true;
}

bool AMMOBaseCharacter::CanAttackTarget(AMMOBaseCharacter* Target) const
{
	if (!Target || !CanCharacterAttack())
		return false;
	
	const float DistanceSq = (GetActorLocation() - Target->GetActorLocation()).SizeSquared();
	return DistanceSq <= MainHandWeapon->Stats.WeaponRange * MainHandWeapon->Stats.WeaponRange;
}

// Called every frame
void AMMOBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
