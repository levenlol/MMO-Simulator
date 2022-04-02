// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystem/Skills/MMOTimeTickSkill.h"

void UMMOTimeTickSkill::Setup(AMMOBaseCharacter* InOwner)
{
	Super::Setup(InOwner);
}

void UMMOTimeTickSkill::CastAbility(const FMMOSkillInputData& Data)
{
	Super::CastAbility(Data);

	// logic part
	CurrentTick = 0;

	CachedInputData = Data;

	StartTick(CachedInputData);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UMMOTimeTickSkill::Tick, Duration / static_cast<float>(TickNumber), true, Duration / static_cast<float>(TickNumber));
}

void UMMOTimeTickSkill::Tick()
{
	Step(CachedInputData, CurrentTick);
	if (++CurrentTick >= TickNumber)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		EndTick(CachedInputData);
	}
}

void UMMOTickSkill::Step(const FMMOSkillInputData& Data, int32 TickCount)
{
	for (UMMOBaseSkill* Skill : TriggeredSkills)
	{
		Skill->CastAbility(Data);
	}
}
