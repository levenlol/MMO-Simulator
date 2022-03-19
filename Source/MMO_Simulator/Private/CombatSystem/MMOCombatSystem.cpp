// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystem/MMOCombatSystem.h"
#include "Characters/MMOBaseCharacter.h"
#include "GameplayTagsManager.h"
#include "Items/MMOBaseWeapon.h"
#include "Core/MMOGameState.h"
#include "CombatSystem/Skills/MMOBaseSkill.h"

UMMOCombatSystem::UMMOCombatSystem()
	: Super()
{
	AttackTag = FMMOStatusTags::Get().AttackTag;
	StunnedTag = FMMOStatusTags::Get().StunnedTag;
	CastTag = FMMOStatusTags::Get().CastTag;

	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UMMOCombatSystem::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<AMMOBaseCharacter>(GetOwner());
	OwnerCharacter->OnEquipWeapon.AddDynamic(this, &UMMOCombatSystem::OnCharacterChangeWeapon);
	OwnerCharacter->OnCharacterStunned.AddDynamic(this, &UMMOCombatSystem::OnCharacterStunned);
}

void UMMOCombatSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (UMMOWrapperSkill* Skill : Skills)
	{
		Skill->Tick(DeltaTime);
	}
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

void UMMOCombatSystem::TryCastSkill(AMMOBaseCharacter* Target, const FVector& Location, const int32 Index)
{
	if (Skills.IsValidIndex(Index) && OwnerCharacter)
	{
		const FVector RangeLocation = Target ? Target->GetActorLocation() : Location;
		const bool bInRange = (OwnerCharacter->GetActorLocation() - RangeLocation).SizeSquared() <= (Skills[Index]->Range * Skills[Index]->Range);

		if (!bInRange)
		{
			OnSkillStartFailed.Broadcast(Skills[Index], EMMOSkillCastFailType::OutOfRange);
			return;
		}

		if (GetRemainingCooldown(Index) > 0.f)
		{
			OnSkillStartFailed.Broadcast(Skills[Index], EMMOSkillCastFailType::Cooldown);
			return;
		}

		if (IsCasting())
		{
			OnSkillStartFailed.Broadcast(Skills[Index], EMMOSkillCastFailType::AlreadyCasting);
			return;
		}

		if (Skills[Index]->IsLocked())
		{
			OnSkillStartFailed.Broadcast(Skills[Index], EMMOSkillCastFailType::Unavailable);
			return;
		}

		const bool bOnSameSide = UMMOGameplayUtils::AreOnTheSameSide(OwnerCharacter, Target);

		const FMMOSkillTags& SkillTags = FMMOSkillTags::Get();
		const bool bCanCastSkill = (Skills[Index]->Tags.HasTag(SkillTags.EnemyTag) && !bOnSameSide && Target)
			|| (Skills[Index]->Tags.HasTag(SkillTags.FriendlyTag) && bOnSameSide && Target)
			|| (Skills[Index]->Tags.HasTag(SkillTags.LocationTag))
			|| (Skills[Index]->Tags.HasTag(SkillTags.Buff));

		if (!bCanCastSkill)
		{
			OnSkillStartFailed.Broadcast(Skills[Index], EMMOSkillCastFailType::WrongTarget);
			return;
		}

		FMMOSkillInputData InputData;
		InputData.TargetLocation = Location;
		InputData.TargetActor = Target;
		InputData.SourceActor = OwnerCharacter;
		InputData.SourceLocation = OwnerCharacter->GetActorLocation();

		Skills[Index]->CastAbility(InputData);

		LastSpellCastTime = GetWorld()->GetTimeSeconds();
		LastAttackTime = LastSpellCastTime;
	}
}

void UMMOCombatSystem::SetSkills(const TArray<TSubclassOf<UMMOWrapperSkill>>& InSkills)
{
	for (UMMOWrapperSkill* Skill : Skills)
	{
		Skill->OnSkillStart.RemoveDynamic(this, &UMMOCombatSystem::OnSkillStart);
		Skill->OnSkillFinish.RemoveDynamic(this, &UMMOCombatSystem::OnSkillFinish);
	}

	Skills.Empty();
	for (TSubclassOf<UMMOWrapperSkill> SkillClass : InSkills)
	{
		UMMOWrapperSkill* Skill = NewObject<UMMOWrapperSkill>(this, SkillClass);
		Skill->Setup(OwnerCharacter);

		Skills.Add(Skill);

		Skill->OnSkillStart.AddDynamic(this, &UMMOCombatSystem::OnSkillStart);
		Skill->OnSkillFinish.AddDynamic(this, &UMMOCombatSystem::OnSkillFinish);
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

bool UMMOCombatSystem::IsCasting() const
{
	return OwnerCharacter ? OwnerCharacter->HasTag(CastTag) : false;
}

bool UMMOCombatSystem::TryAttack(AMMOBaseCharacter* Target)
{
	if (!CanAttackTarget(Target) || !IsAttacking())
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

void UMMOCombatSystem::OnSkillStart(UMMOWrapperSkill* Sender)
{
	OwnerCharacter->GiveTag(CastTag);
}

void UMMOCombatSystem::OnSkillFinish(UMMOWrapperSkill* Sender)
{
	OwnerCharacter->RemoveTag(CastTag);
}

void UMMOCombatSystem::OnCharacterStunned(AMMOBaseCharacter* Sender)
{
	// we clearly have to stop everything we are doing.
	for (UMMOWrapperSkill* Skill : Skills)
	{
		Skill->AbortAbility();
	}

	OwnerCharacter->RemoveTag(AttackTag);
	OwnerCharacter->RemoveTag(CastTag);
}

bool UMMOCombatSystem::CanCharacterAttack() const
{
	if (!OwnerCharacter)
		return false;

	const float CurrentTime = GetWorld()->GetTimeSeconds();

	const bool bIsStunned = IsStunned();
	const bool bIsAttacking = IsAttacking();
	const AMMOBaseWeapon* MainHandWeapon = GetEquippedMainHandWeapon();
	return MainHandWeapon && !bIsAttacking && !bIsStunned && (GetWorld()->GetTimeSeconds() - LastAttackTime) > MainHandWeapon->Stats.AttackSpeed;
}

float UMMOCombatSystem::GetRemainingGlobalCooldown() const
{
	if (GlobalCooldown <= 0.f)
		return 0.f;

	const float CurrentTime = GetWorld()->GetTimeSeconds();

	const float Delta = (CurrentTime - LastSpellCastTime);
	return FMath::Max(0.f, GlobalCooldown - Delta);
}

float UMMOCombatSystem::GetRemainingCooldown(int32 SpellIndex) const
{
	if (Skills.IsValidIndex(SpellIndex) && Skills[SpellIndex]->IsInCooldown())
	{
		// if spell is in cooldown we also check for global cooldown as it might be higher than the remaining global cooldown
		return FMath::Max(Skills[SpellIndex]->GetRemainingCooldown(), GetRemainingGlobalCooldown());
	}

	return GetRemainingGlobalCooldown();
}
