// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystem/MMOBaseSkill.h"

FMMOSkillTags FMMOSkillTags::SkillTags;

void UMMOBaseSkill::Setup(AMMOBaseCharacter* InOwner)
{
	OwnerCharacter = InOwner;

	for (UMMOBaseSkill* Skill : TriggeredSkills)
	{
		Skill->Setup(InOwner);
	}
}

const UMMOWrapperSkill* UMMOBaseSkill::GetOuterSkill() const
{
	return Cast<UMMOWrapperSkill>(GetOuterSkill_Rec(this));
}

const UMMOBaseSkill* UMMOBaseSkill::GetOuterSkill_Rec(const UMMOBaseSkill* InSkill) const
{
	if (InSkill)
	{
		const UMMOBaseSkill* OuterSkill = Cast<UMMOBaseSkill>(InSkill->GetOuter());
		return OuterSkill ? GetOuterSkill_Rec(OuterSkill) : InSkill;
	}

	return nullptr;
}

void UMMOWrapperSkill::Setup(AMMOBaseCharacter* InOwner)
{
	Super::Setup(InOwner);

	LastCastTime = -Cooldown;// Start with no cooldown
}

void UMMOWrapperSkill::CastAbility(FMMOSkillInputData Data)
{
	if (IsInCooldown())
	{
		UE_LOG(LogTemp, Warning, TEXT("Requested to cast %s ability but failed because it was in cooldown."), *GetName());
		return;
	}

	if (IsLocked())
	{
		UE_LOG(LogTemp, Warning, TEXT("Requested to cast %s ability but failed because it is locked."), *GetName());
		return;
	}

	if (IsCasting())
	{
		UE_LOG(LogTemp, Warning, TEXT("Requested to cast %s ability but failed because it is already being casted."), *GetName());
		return;
	}

	SavedInputData = Data;

	if (FMath::IsNearlyZero(CastTime, KINDA_SMALL_NUMBER))
	{
		FinishCastAbility();
	}
	else
	{
		// start cast
		CurrentCastingTime = 0.f;
		bCasting = true;
	}
}

void UMMOWrapperSkill::Tick(float DeltaSeconds)
{
	if (!bCasting)
		return;

	CurrentCastingTime += DeltaSeconds;

	if (CurrentCastingTime >= CastTime)
	{
		FinishCastAbility();

		CurrentCastingTime = CastTime;
		bCasting = false;
	}
}

bool UMMOWrapperSkill::IsInCooldown() const
{
	const float CurrentTime = GetWorld()->GetTimeSeconds();

	return CurrentTime - LastCastTime <= Cooldown;
}

float UMMOWrapperSkill::GetCooldownPercent() const
{
	if (IsInCooldown())
	{
		const float CurrentTime = GetWorld()->GetTimeSeconds();

		return FMath::Clamp((CurrentTime - LastCastTime) / Cooldown, 0.f, 1.f);
	}

	return 1.0f;
}

float UMMOWrapperSkill::GetCastingPercent() const
{
	if (bCasting)
	{
		return CurrentCastingTime / CastTime;
	}

	return 0.0f;
}

void UMMOWrapperSkill::FinishCastAbility()
{
	LastCastTime = GetWorld()->GetTimeSeconds();

	for (UMMOBaseSkill* Skill : TriggeredSkills)
	{
		Skill->CastAbility(SavedInputData);
	}
}

