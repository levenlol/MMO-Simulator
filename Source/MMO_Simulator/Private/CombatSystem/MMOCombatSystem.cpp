// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystem/MMOCombatSystem.h"
#include "Characters/MMOBaseCharacter.h"
#include "GameplayTagsManager.h"
#include "Items/MMOBaseWeapon.h"
#include "Core/MMOGameState.h"
#include "Data/MMOStatsManager.h"
#include "CombatSystem/Skills/MMOBaseSkill.h"
#include "CombatSystem/MMOWrapperSkill.h"

UMMOCombatSystem::UMMOCombatSystem()
	: Super()
{
	AttackTag = FMMOStatusTags::Get().AttackTag;
	StunnedTag = FMMOStatusTags::Get().StunnedTag;
	CastTag = FMMOStatusTags::Get().CastTag;
	ChannelingTag = FMMOStatusTags::Get().ChannelingTag;

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

void UMMOCombatSystem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (OwnerCharacter)
	{
		OwnerCharacter->OnEquipWeapon.RemoveDynamic(this, &UMMOCombatSystem::OnCharacterChangeWeapon);
		OwnerCharacter->OnCharacterStunned.RemoveDynamic(this, &UMMOCombatSystem::OnCharacterStunned);
		OwnerCharacter = nullptr;
	}

	for (UMMOWrapperSkill* Skill : Skills)
	{
		Skill->End();
	}
}

void UMMOCombatSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsAttacking())
	{
		if (AMMOBaseWeapon* Weapon = GetEquippedMainHandWeapon())
		{
			const float CurrentGameTime = GetWorld()->GetTimeSeconds();

			if (CurrentGameTime - LastAttackTime >= Weapon->Stats.AttackSpeed)
			{
				if (AutoAttackTarget && CanAttackTarget(AutoAttackTarget) && IsAttacking())
				{
					if (IsCasting() || IsChanneling())
					{
						StopAttack();
					}
					else if (IsStunned() || TryAttack(AutoAttackTarget))
					{
						LastAttackTime = CurrentGameTime;
					}
				}
			}
			else if (DelayAADamage.HasPendingDamage())
			{
				DelayAADamage.TimeToDamage -= DeltaTime;
				if (DelayAADamage.TimeToDamage <= 0.f)
				{
					DoAutoAttackDamage(DelayAADamage.Target);
					DelayAADamage.Clear();
				}
			}
		}
	}
	
	for (UMMOWrapperSkill* Skill : Skills)
	{
		Skill->Tick(DeltaTime);
	}
}

bool UMMOCombatSystem::StartAttack(AMMOBaseCharacter* Target)
{
	if (!IsValid(OwnerCharacter) || !OwnerCharacter->GetMainHandWeapon())
	{
		return false;
	}

	if (Target == AutoAttackTarget) return true;

	OwnerCharacter->GiveTag(AttackTag);

	AutoAttackTarget = Target;
	return true;
}

bool UMMOCombatSystem::CanAttackTarget(AMMOBaseCharacter* Target) const
{
	if (!Target) return false;

	const float DistanceSq = (OwnerCharacter->GetActorLocation() - Target->GetActorLocation()).SizeSquared2D();

	float TargetRadius, TargetHalfheight;
	Target->GetSimpleCollisionCylinder(TargetRadius, TargetHalfheight);

	float OwnerRadius, OwnerHalfheight;
	OwnerCharacter->GetSimpleCollisionCylinder(OwnerRadius, OwnerHalfheight);

	const AMMOBaseWeapon* MainHandWeapon = GetEquippedMainHandWeapon();

	const float EffectiveRange = MainHandWeapon->Stats.WeaponRange + OwnerRadius + TargetRadius;
	return DistanceSq <= EffectiveRange * EffectiveRange;
}

void UMMOCombatSystem::TryCastSkill(AMMOBaseCharacter* Target, const FVector& Location, const int32 Index)
{
	EMMOSkillCastFailType FailCastType = CanCastSkill(Target, Location, Index);
	if (FailCastType != EMMOSkillCastFailType::None)
	{
		OnSkillStartFailed.Broadcast(Skills[Index], FailCastType);
		return;
	}

	FMMOSkillInputData InputData;
	InputData.TargetLocation = Location;
	InputData.TargetActor = Target;
	InputData.SourceActor = OwnerCharacter;
	InputData.SourceLocation = OwnerCharacter->GetActorLocation();

	Skills[Index]->TryCastAbility(InputData);

	LastSpellCastTime = GetWorld()->GetTimeSeconds();
	LastAttackTime = LastSpellCastTime;
}

void UMMOCombatSystem::AbortCastSkill()
{
	for (UMMOWrapperSkill* WrapperSkill : Skills)
	{
		WrapperSkill->AbortAbility();
	}

	LastSpellCastTime = GetWorld()->GetTimeSeconds();
	LastAttackTime = LastSpellCastTime;
}

EMMOSkillCastFailType UMMOCombatSystem::CanCastSkill(AMMOBaseCharacter* Target, const FVector& Location, const int32 Index) const
{
	if (Skills.IsValidIndex(Index) && OwnerCharacter)
	{
		const FVector RangeLocation = Target ? Target->GetActorLocation() : Location;

		const float SkillRange = Skills[Index]->Range;

		float TargetRadius = 0.f, OwnerRadius, UnusuedHalfHeight;
		
		if (Target)
		{
			Target->GetSimpleCollisionCylinder(TargetRadius, UnusuedHalfHeight);
		}
		OwnerCharacter->GetSimpleCollisionCylinder(OwnerRadius, UnusuedHalfHeight);

		const float ActualRange = SkillRange + TargetRadius + OwnerRadius;
		const bool bInRange = (OwnerCharacter->GetActorLocation() - RangeLocation).SizeSquared2D() <= (ActualRange * ActualRange);

		if (!bInRange)
		{
			return EMMOSkillCastFailType::OutOfRange;
		}

		if (GetRemainingCooldown(Index) > 0.f)
		{
			return EMMOSkillCastFailType::Cooldown;
		}

		if (IsCasting())
		{
			return EMMOSkillCastFailType::AlreadyCasting;
		}

		if (Skills[Index]->IsLocked())
		{
			return EMMOSkillCastFailType::Unavailable;
		}

		const bool bOnSameSide = UMMOGameplayUtils::AreOnTheSameSide(OwnerCharacter, Target);

		const FMMOSkillTags& SkillTags = FMMOSkillTags::Get();
		const bool bCanCastSkill = (Skills[Index]->Tags.HasTag(SkillTags.EnemyTag) && !bOnSameSide && Target)
			|| (Skills[Index]->Tags.HasTag(SkillTags.FriendlyTag) && bOnSameSide && Target)
			|| (Skills[Index]->Tags.HasTag(SkillTags.LocationTag))
			|| (Skills[Index]->Tags.HasTag(SkillTags.Buff));

		if (!bCanCastSkill)
		{
			return EMMOSkillCastFailType::WrongTarget;
		}

		return EMMOSkillCastFailType::None;
	}

	return EMMOSkillCastFailType::Unspecified;
}

void UMMOCombatSystem::SetSkills(const TArray<TSubclassOf<UMMOWrapperSkill>>& InSkills)
{
	for (UMMOWrapperSkill* Skill : Skills)
	{
		Skill->OnSkillStartCast.RemoveDynamic(this, &UMMOCombatSystem::OnSkillStartCast);
		Skill->OnSkillFinishCast.RemoveDynamic(this, &UMMOCombatSystem::OnSkillFinishCast);
		Skill->OnSkillStartChanneling.RemoveDynamic(this, &UMMOCombatSystem::OnSkillStartChanneling);
		Skill->OnSkillFinishChanneling.RemoveDynamic(this, &UMMOCombatSystem::OnSkillFinishChanneling);
		Skill->OnSkillAborted.RemoveDynamic(this, &UMMOCombatSystem::OnSkillAbort);
	}

	Skills.Empty();
	for (TSubclassOf<UMMOWrapperSkill> SkillClass : InSkills)
	{
		UMMOWrapperSkill* Skill = NewObject<UMMOWrapperSkill>(this, SkillClass);
		Skill->Setup(OwnerCharacter);

		Skills.Add(Skill);

		Skill->OnSkillStartCast.AddDynamic(this, &UMMOCombatSystem::OnSkillStartCast);
		Skill->OnSkillFinishCast.AddDynamic(this, &UMMOCombatSystem::OnSkillFinishCast);
		Skill->OnSkillStartChanneling.AddDynamic(this, &UMMOCombatSystem::OnSkillStartChanneling);
		Skill->OnSkillFinishChanneling.AddDynamic(this, &UMMOCombatSystem::OnSkillFinishChanneling);
		Skill->OnSkillAborted.AddDynamic(this, &UMMOCombatSystem::OnSkillAbort);
	}
}

bool UMMOCombatSystem::IsAttacking() const
{
	if (OwnerCharacter)
	{
		const bool bAttackTag = OwnerCharacter->HasTag(AttackTag);

		return bAttackTag && !IsStunned() && !IsCasting() && !IsChanneling();
	}
	return false;
}

bool UMMOCombatSystem::IsStunned() const
{
	return OwnerCharacter ? OwnerCharacter->HasTag(StunnedTag) : false;
}

bool UMMOCombatSystem::IsCasting() const
{
	return OwnerCharacter ? OwnerCharacter->HasTag(CastTag) : false;
}

bool UMMOCombatSystem::IsChanneling() const
{
	return OwnerCharacter ? OwnerCharacter->HasTag(ChannelingTag) : false;
}

bool UMMOCombatSystem::TryAttack(AMMOBaseCharacter* Target)
{
	if (!IsAttacking() || !Target)
	{
		StopAttack();
		return false;
	}

	DelayAADamage.DelayDamage(Target, GetEquippedMainHandWeapon()->Stats.AttackSpeed / 2.f);

	if (AMMOGameState* GameState = AMMOGameState::GetMMOGameState(this))
	{
		GameState->NotifyStartAttack(OwnerCharacter);
	}

	return true;
}

void UMMOCombatSystem::DoAutoAttackDamage(AMMOBaseCharacter* Target)
{
	if (!Target)
		return;

	if (Target->IsAlive())
	{
		FMMODamage Damage = ComputeAutoAttackDamage();
		OwnerCharacter->DoDamage(MoveTemp(Damage), Target);
	}
}

void UMMOCombatSystem::StopAttack()
{
	if (!IsAttacking())
	{
		return;
	}

	OwnerCharacter->RemoveTag(AttackTag);

	const float CurrentGameTime = GetWorld()->GetTimeSeconds();
	LastAttackTime = CurrentGameTime;
	AutoAttackTarget = nullptr;
}

FMMODamage UMMOCombatSystem::ComputeAutoAttackDamage()
{
	FMMODamage Damage;
	if (AMMOBaseWeapon* MainHandWeapon = OwnerCharacter->GetMainHandWeapon())
	{
		const FMMOWeaponStats& WeaponStats = MainHandWeapon->Stats;
		Damage.Damage = FMath::RandRange(WeaponStats.DamageMin, WeaponStats.DamageMax);
		
		const float CritChance = OwnerCharacter->StatsManager->GetAttackCritChance();
		Damage.bCrit = CritChance >= FMath::RandRange(0.f, 1.f);

		if (Damage.bCrit)
		{
			Damage.Damage *= 2; // todo: get crit multiplier
		}

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

void UMMOCombatSystem::OnSkillStartCast(UMMOWrapperSkill* Sender)
{
	OwnerCharacter->GiveTag(CastTag);
}

void UMMOCombatSystem::OnSkillFinishCast(UMMOWrapperSkill* Sender)
{
	OwnerCharacter->RemoveTag(CastTag);
}

void UMMOCombatSystem::OnSkillStartChanneling(UMMOWrapperSkill* Sender)
{
	OwnerCharacter->GiveTag(ChannelingTag);
}

void UMMOCombatSystem::OnSkillFinishChanneling(UMMOWrapperSkill* Sender)
{
	OwnerCharacter->RemoveTag(ChannelingTag);
}

void UMMOCombatSystem::OnSkillAbort(UMMOWrapperSkill* Sender)
{
	OwnerCharacter->RemoveTag(CastTag);
	OwnerCharacter->RemoveTag(ChannelingTag);
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
	OwnerCharacter->RemoveTag(ChannelingTag);
}

bool UMMOCombatSystem::CanCharacterAttack() const
{
	if (!OwnerCharacter)
		return false;

	const float CurrentTime = GetWorld()->GetTimeSeconds();

	const bool bIsStunned = IsStunned();
	const bool bIsAttacking = IsAttacking();
	const bool bIsCasting = IsCasting();
	const AMMOBaseWeapon* MainHandWeapon = GetEquippedMainHandWeapon();
	return MainHandWeapon && !bIsAttacking && !bIsCasting && !bIsStunned;
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

bool UMMOCombatSystem::IsSkillReady(int32 SpellIndex) const
{
	if (Skills.IsValidIndex(SpellIndex) )
	{
		return Skills[SpellIndex]->IsSkillReady();
	}

	return false;
}

void FMMODelayAADamage::DelayDamage(AMMOBaseCharacter* InTarget, float Delay)
{
	Target = InTarget;
	if (!Target)
	{
		Clear();
		return;
	}

	TimeToDamage = Delay;
	bPending = true;
}

void FMMODelayAADamage::Clear()
{
	Target = nullptr;
	TimeToDamage = 0.f;
	bPending = false;
}
