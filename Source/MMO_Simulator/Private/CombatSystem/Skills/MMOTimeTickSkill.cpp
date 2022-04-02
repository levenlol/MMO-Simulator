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

	InputData = Data;

	StartTick(InputData);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UMMOTimeTickSkill::Tick, Duration / static_cast<float>(TickNumber), true, Duration / static_cast<float>(TickNumber));
}

void UMMOTimeTickSkill::Tick()
{
	Step(InputData, CurrentTick);
	if (++CurrentTick >= TickNumber)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		EndTick(InputData);
	}
}

void UMMOTickSkill::Step(const FMMOSkillInputData& Data, int32 TickCount)
{
	for (UMMOBaseSkill* Skill : TriggeredSkills)
	{
		Skill->CastAbility(Data);
	}
}
