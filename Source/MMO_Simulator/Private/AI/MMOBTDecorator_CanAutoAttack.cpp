// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MMOBTDecorator_CanAutoAttack.h"
#include "Characters/MMOBaseCharacter.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"

bool UMMOBTDecorator_CanAutoAttack::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AMMOBaseCharacter* Character = Cast<AMMOBaseCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (Character)
	{
		return Character->CanCharacterAttack();
	}

	return false;
}
