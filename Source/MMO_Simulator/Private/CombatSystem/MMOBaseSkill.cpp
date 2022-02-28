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

void UMMOBaseSkill::Tick(float DeltaSeconds)
{
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

	LastCastTime = GetWorld()->GetTimeSeconds();
	for (UMMOBaseSkill* Skill : TriggeredSkills)
	{
		Skill->CastAbility(Data);
	}
}

bool UMMOWrapperSkill::IsInCooldown() const
{
	const float CurrentTime = GetWorld()->GetTimeSeconds();

	return CurrentTime - LastCastTime <= Cooldown;
}

