// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorators/MMOBTDecorator_IsCasting.h"
#include "Characters/MMOBaseCharacter.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CombatSystem/MMOCombatSystem.h"

UMMOBTDecorator_IsCasting::UMMOBTDecorator_IsCasting()
	: Super()
{
	NodeName = "Is Casting";
}

bool UMMOBTDecorator_IsCasting::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AMMOBaseCharacter* BaseCharacter = Cast<AMMOBaseCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();

	if (!BaseCharacter || !BlackBoard)
	{
		return false;
	}

	const int32 SpellRequest = BlackBoard->GetValueAsInt(SpellRequestedSelectorKey.SelectedKeyName) - 1;

	if (BaseCharacter->CombatSystem->Skills.IsValidIndex(SpellRequest))
	{
		return true;
	}

	return BaseCharacter->CombatSystem->IsCasting();
}
