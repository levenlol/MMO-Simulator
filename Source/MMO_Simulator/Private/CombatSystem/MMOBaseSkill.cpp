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

