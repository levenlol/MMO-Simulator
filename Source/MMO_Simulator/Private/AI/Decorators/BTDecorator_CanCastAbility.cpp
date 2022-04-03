// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorators/BTDecorator_CanCastAbility.h"
#include "Characters/MMOBaseCharacter.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CombatSystem/MMOCombatSystem.h"
#include "CombatSystem/MMOWrapperSkill.h"

UBTDecorator_CanCastAbility::UBTDecorator_CanCastAbility()
	: Super()
{
}

bool UBTDecorator_CanCastAbility::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AMMOBaseCharacter* BaseCharacter = Cast<AMMOBaseCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();

	if (!BaseCharacter || !BlackBoard || !BaseCharacter->CombatSystem)
	{
		return false;
	}

	const bool bCasting = BaseCharacter->CombatSystem->IsCasting();
	const bool bStunned = BaseCharacter->CombatSystem->IsStunned();
	const bool bGlobalCooldown = BaseCharacter->CombatSystem->GetRemainingGlobalCooldown() > 0.f;

	if (bCasting || bStunned)
	{
		return false;
	}

	// Check if we already have a set spell to be processed later. It also might be set from the player
	const int32 SpellRequest = BlackBoard->GetValueAsInt(SpellRequestedSelectorKey.SelectedKeyName) - 1;

	if (BaseCharacter->CombatSystem->Skills.IsValidIndex(SpellRequest))
	{
		return BaseCharacter->CombatSystem->IsSkillReady(SpellRequest);
	}

	float Range = 0.f;
	for (const UMMOWrapperSkill* Skill : BaseCharacter->CombatSystem->Skills)
	{
		if (Skill->GetRemainingCooldown() <= 0.f)
		{
			Range = FMath::Max(Skill->Range, Range);
		}
	}

	return Range > 0.f;
}

