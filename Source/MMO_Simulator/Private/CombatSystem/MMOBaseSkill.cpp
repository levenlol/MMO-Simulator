// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystem/MMOBaseSkill.h"

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

void UMMOWrapperSkill::CastAbility(FMMOSkillInputData Data)
{
	for (UMMOBaseSkill* Skill : TriggeredSkills)
	{
		Skill->CastAbility(Data);
	}
}

bool UMMOWrapperSkill::IsInCooldown() const
{
	// TODO
	return false;
}

