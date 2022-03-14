// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MMOAIController.h"
#include "Characters/MMOBaseCharacter.h"
#include "CombatSystem/MMOCombatSystem.h"

void AMMOAIController::RequestCastSpell(int32 SpellIndex)
{
	if (AMMOBaseCharacter* MMOCharacter = Cast<AMMOBaseCharacter>(GetPawn()))
	{
		//Character->CombatSystem->
	}
}
