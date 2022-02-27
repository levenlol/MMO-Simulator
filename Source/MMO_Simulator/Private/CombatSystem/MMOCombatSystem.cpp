// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystem/MMOCombatSystem.h"
#include "Characters/MMOBaseCharacter.h"
#include "GameplayTagsManager.h"
#include "Weapons/MMOBaseWeapon.h"
#include "Core/MMOGameState.h"
#include "CombatSystem/MMOBaseSkill.h"

UMMOCombatSystem::UMMOCombatSystem()
	: Super()
{
	AttackTag = UGameplayTagsManager::Get().RequestGameplayTag(FName("Status.Action.Attack"));
	StunnedTag = UGameplayTagsManager::Get().RequestGameplayTag(FName("Status.Malus.Stunned"));
}

void UMMOCombatSystem::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<AMMOBaseCharacter>(GetOwner());
	OwnerCharacter->OnEquipWeapon.AddDynamic(this, &UMMOCombatSystem::OnCharacterChangeWeapon);
}

void UMMOCombatSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UMMOCombatSystem::StartAttack(AMMOBaseCharacter* Target)
{
	if (!CanAttackTarget(Target) || !IsValid(OwnerCharacter) || !OwnerCharacter->HasEquippedAnyWeapons())
	{
		return false;
	}

	OwnerCharacter->GiveTag(AttackTag);

	// Damage Delegate
	FTimerHandle UnusuedHandle;
	FTimerDelegate AttackDelegate;

	TWeakObjectPtr<AMMOBaseCharacter> TargetWeak = Target;
	TWeakObjectPtr<UMMOCombatSystem> ThisWeak = this;

	AttackDelegate.BindLambda([ThisWeak, TargetWeak]()
	{
		if (!ThisWeak.IsValid())
		{
			return;
		}

		ThisWeak->TryAttack(TargetWeak.Get());
	});

	// Stop autoattack delegate
	FTimerHandle UnusuedHandle2;

	AMMOBaseWeapon* MainHandWeapon = GetEquippedMainHandWeapon();
	GetWorld()->GetTimerManager().SetTimer(UnusuedHandle, AttackDelegate, MainHandWeapon->Stats.AttackSpeed * 0.5f, false, MainHandWeapon->Stats.AttackSpeed * 0.5f);
	GetWorld()->GetTimerManager().SetTimer(UnusuedHandle2, this, &UMMOCombatSystem::StopAttack, MainHandWeapon->Stats.AttackSpeed, false, MainHandWeapon->Stats.AttackSpeed);

	return true;
}

bool UMMOCombatSystem::CanAttackTarget(AMMOBaseCharacter* Target) const
{
	if (!Target || !CanCharacterAttack())
		return false;

	const float DistanceSq = (OwnerCharacter->GetActorLocation() - Target->GetActorLocation()).SizeSquared();

	const AMMOBaseWeapon* MainHandWeapon = GetEquippedMainHandWeapon();
	return DistanceSq <= MainHandWeapon->Stats.WeaponRange * MainHandWeapon->Stats.WeaponRange;
}

void UMMOCombatSystem::TryCastSkill(AActor* Target, const FVector& Location, const int32 Index)
{
	if (Skills.IsValidIndex(Index) && OwnerCharacter)
	{
		FMMOSkillInputData InputData;
		InputData.Location = Location;
		InputData.TargetActor = Target;

		const bool bInRange = (OwnerCharacter->GetActorLocation() - Location).SizeSquared() <= (Skills[Index]->Range * Skills[Index]->Range);

		if (bInRange)
		{
			Skills[Index]->CastAbility(InputData);
		}
	}
}

void UMMOCombatSystem::SetSkills(const TArray<TSubclassOf<UMMOWrapperSkill>>& InSkills)
{
	Skills.Empty();
	for (TSubclassOf<UMMOWrapperSkill> SkillClass : InSkills)
	{
		UMMOWrapperSkill* Skill = NewObject<UMMOWrapperSkill>(this, SkillClass);
		Skill->Setup(OwnerCharacter);

		Skills.Add(Skill);
	}
}

bool UMMOCombatSystem::IsAttacking() const
{
	return OwnerCharacter ? OwnerCharacter->HasTag(AttackTag) : false;
}

bool UMMOCombatSystem::IsStunned() const
{
	return OwnerCharacter ? OwnerCharacter->HasTag(StunnedTag) : false;
}

bool UMMOCombatSystem::TryAttack(AMMOBaseCharacter* Target)
{
	if (!CanAttackTarget(Target))
	{
		StopAttack();
		return false;
	}

	FMMODamage Damage = ComputeAutoAttackDamage();
	Target->DamageTake(MoveTemp(Damage));

	if (AMMOGameState* GameState = AMMOGameState::GetMMOGameState(this))
	{
		GameState->NotifyStartAttack(OwnerCharacter);
	}

	return true;
}

void UMMOCombatSystem::StopAttack()
{
	if (!IsAttacking())
	{
		return;
	}

	OwnerCharacter->RemoveTag(AttackTag);
}

FMMODamage UMMOCombatSystem::ComputeAutoAttackDamage()
{
	FMMODamage Damage;
	if (AMMOBaseWeapon* MainHandWeapon = OwnerCharacter->GetMainHandWeapon())
	{
		const FMMOWeaponStats& WeaponStats = MainHandWeapon->Stats;
		Damage.Damage = FMath::RandRange(WeaponStats.Damage.X, WeaponStats.Damage.Y);
		Damage.bCrit = WeaponStats.CritChance >= FMath::RandRange(0.f, 1.f);
		Damage.DamageType = EMMODamageType::Physical;
		Damage.DamageDealer = OwnerCharacter;
	}

	return Damage;
}

AMMOBaseWeapon* UMMOCombatSystem::GetEquippedMainHandWeapon() const
{
	if (!OwnerCharacter)
		return nullptr;

	return OwnerCharacter->GetMainHandWeapon();
}

AMMOBaseWeapon* UMMOCombatSystem::GetEquippedOffHandWeapon() const
{
	if (!OwnerCharacter)
		return nullptr;

	return OwnerCharacter->GetOffHandWeapon();
}

void UMMOCombatSystem::OnCharacterChangeWeapon(AMMOBaseCharacter* Sender, AMMOBaseWeapon* New, AMMOBaseWeapon* Old)
{
	LastAttackTime = GetWorld()->GetTimeSeconds();
}

bool UMMOCombatSystem::CanCharacterAttack() const
{
	if (!OwnerCharacter)
		return false;

	const float CurrentTime = GetWorld()->GetTimeSeconds();

	const bool bIsStunned = IsStunned();
	const AMMOBaseWeapon* MainHandWeapon = GetEquippedMainHandWeapon();
	return MainHandWeapon && !bIsStunned && (GetWorld()->GetTimeSeconds() - LastAttackTime) > MainHandWeapon->Stats.AttackSpeed;
}