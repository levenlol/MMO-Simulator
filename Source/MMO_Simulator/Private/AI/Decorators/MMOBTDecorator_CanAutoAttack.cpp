// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorators/MMOBTDecorator_CanAutoAttack.h"
#include "Characters/MMOBaseCharacter.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CombatSystem/MMOCombatSystem.h"


bool UMMOBTDecorator_CanAutoAttack::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AMMOBaseCharacter* Character = Cast<AMMOBaseCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	const UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if (IsValid(Character) && BlackBoard)
	{
		AMMOBaseCharacter* Target = Cast<AMMOBaseCharacter>(BlackBoard->GetValueAsObject(FName("TargetActor")));
		return Target && Character->CombatSystem->CanAttackTarget(Target);
	}

	return false;
}
