// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystem/Skills/MMOTimeTickSkill.h"

void UMMOTimeTickSkill::Setup(AMMOBaseCharacter* InOwner)
{
	Super::Setup(InOwner);
}

void UMMOTimeTickSkill::CastAbility(FMMOSkillInputData Data)
{
	Super::CastAbility(Data);

	// logic part
	CurrentTick = 0;

	StartTick();

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UMMOTimeTickSkill::Tick, Duration / static_cast<float>(TickNumber), true, Duration / static_cast<float>(TickNumber));
}

void UMMOTimeTickSkill::Tick()
{
	Step(CurrentTick);
	if (++CurrentTick >= TickNumber)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		EndTick();
	}
}
