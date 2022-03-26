// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorators/MMOBTDecorator_HasAggro.h"
#include "Characters/MMOBaseEnemy.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "AI/MMOAggroManagerComponent.h"


bool UMMOBTDecorator_HasAggro::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AMMOBaseEnemy* Character = Cast<AMMOBaseEnemy>(OwnerComp.GetAIOwner()->GetPawn());

	if (IsValid(Character))
	{
		return Character->AggroManager->GetSortedAggroList().Num() > 0;
	}

	return false;
}
