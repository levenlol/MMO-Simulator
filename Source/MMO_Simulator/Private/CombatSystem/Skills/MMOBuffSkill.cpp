// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystem/Skills/MMOBuffSkill.h"
#include "Characters/MMOBaseCharacter.h"
#include "CombatSystem/Buffs/MMOBaseBuff.h"
#include "CombatSystem/MMOBuffManager.h"


void UMMOBuffSkill::Setup(AMMOBaseCharacter* InOwner)
{
	Super::Setup(InOwner);

	Buff = NewObject<UMMOBaseBuff>(this, BuffClass);
	Buff->Init(OwnerCharacter);
}

void UMMOBuffSkill::CastAbility(const FMMOSkillInputData& Data)
{
	Super::CastAbility(Data);

	if (Data.TargetActor && Buff)
	{
		Data.TargetActor->BuffManager->AddBuff(Buff, Duration);
	}
}
