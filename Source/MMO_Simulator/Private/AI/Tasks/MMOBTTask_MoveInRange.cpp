// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/MMOBTTask_MoveInRange.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Characters/MMOBaseCharacter.h"
#include "Items/MMOBaseWeapon.h"
#include "CombatSystem/Skills/MMOBaseSkill.h"
#include "CombatSystem/MMOCombatSystem.h"

UMMOBTTask_MoveInRange::UMMOBTTask_MoveInRange()
	: Super()
{
	NodeName = "Move in Range";

	bReachTestIncludesAgentRadius = true;
	bReachTestIncludesGoalRadius = true;
	bAllowPartialPath = true;
	bTrackMovingGoal = true;
	bProjectGoalLocation = true;
}

EBTNodeResult::Type UMMOBTTask_MoveInRange::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AMMOBaseCharacter* BaseCharacter = Cast<AMMOBaseCharacter>(OwnerComp.GetAIOwner()->GetPawn());

	AcceptableRadius = DefaultAcceptableRadius;
	if (!BaseCharacter)
	{
		return EBTNodeResult::Type::Failed;
	}

	if (AMMOBaseWeapon* MainHandWeapon = BaseCharacter->GetMainHandWeapon())
	{
		AcceptableRadius = MainHandWeapon->Stats.WeaponRange;
	}

	AMMOBaseWeapon* OffHandWeapon = BaseCharacter->GetOffHandWeapon();
	if (OffHandWeapon && OffHandWeapon->Stats.WeaponRange < AcceptableRadius)
	{
		AcceptableRadius = OffHandWeapon->Stats.WeaponRange;
	}

	for (UMMOWrapperSkill* Skill : BaseCharacter->CombatSystem->Skills)
	{
		AcceptableRadius = FMath::Min(AcceptableRadius, Skill->Range);
	}

	AcceptableRadius *= (1.f - MarginPercent);

	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
