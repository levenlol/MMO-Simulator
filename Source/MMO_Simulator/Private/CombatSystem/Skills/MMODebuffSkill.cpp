// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystem/Skills/MMODebuffSkill.h"
#include "CombatSystem/Debuffs/MMOBaseDebuff.h"
#include "CombatSystem/MMODebuffManager.h"
#include "Characters/MMOBaseCharacter.h"

void UMMODebuffSkill::Setup(AMMOBaseCharacter* InOwner)
{
	Super::Setup(InOwner);

	Debuff = NewObject<UMMOBaseDebuff>(OwnerCharacter, DebuffClass);
	Debuff->Init(OwnerCharacter);
}

void UMMODebuffSkill::CastAbility(const FMMOSkillInputData& Data)
{
	Super::CastAbility(Data);

	if(Data.TargetActor)
	{
		Data.TargetActor->DebuffManager->AddDebuff(Debuff, Duration);
	}
}
