// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystem/Skills/MMOTauntSkill.h"
#include "Characters/MMOBaseEnemy.h"
#include "AI/MMOAggroManagerComponent.h"

void UMMOTauntSkill::CastAbility(const FMMOSkillInputData& Data)
{
	Super::CastAbility(Data);

	if (AMMOBaseEnemy* Enemy = Cast<AMMOBaseEnemy>(Data.TargetActor))
	{
		Enemy->AggroManager->Taunt(OwnerCharacter);
	}
}
