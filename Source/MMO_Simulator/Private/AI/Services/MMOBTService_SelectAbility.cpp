// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/MMOBTService_SelectAbility.h"
#include "Characters/MMOBaseCharacter.h"
#include "AI/MMOAIController.h"
#include "CombatSystem/MMOCombatSystem.h"
#include "CombatSystem/Skills/MMOBaseSkill.h"
#include "BehaviorTree/BlackboardComponent.h"

UMMOBTService_SelectAbility::UMMOBTService_SelectAbility()
	: Super()
{
	NodeName = "Select Ability";

	INIT_SERVICE_NODE_NOTIFY_FLAGS();
	bNotifyTick = true;

}

void UMMOBTService_SelectAbility::OnSearchStart(FBehaviorTreeSearchData& SearchData)
{
	AMMOAIController* AIController = Cast<AMMOAIController>(SearchData.OwnerComp.GetAIOwner());
	if (!AIController)
		return;

	AMMOBaseCharacter* Character = Cast<AMMOBaseCharacter>(AIController->GetPawn());
	UBlackboardComponent* BlackBoard = AIController->GetBlackboardComponent();

	if (!Character || !BlackBoard)
		return;

	const int32 SelectedSpell = BlackBoard->GetValueAsInt(SpellSelector.SelectedKeyName) - 1;
	if (Character->CombatSystem->Skills.IsValidIndex(SelectedSpell))
	{
		// we already have a spell to cast.
		return;
	}

	for (int32 i = 0; i < Character->CombatSystem->Skills.Num(); i++)
	{
		UMMOWrapperSkill* WrapperSkill = Character->CombatSystem->Skills[i];
		if (Character->CombatSystem->GetRemainingCooldown(i) > 0.f || WrapperSkill->IsLocked() || AIController->IsSpellHandledByThePlayer(i))
			continue;

		// TODO: better pick spell logic.
		const FMMOSkillTags& SkillTags = FMMOSkillTags::Get();
		if (WrapperSkill->Tags.HasTag(SkillTags.TargetTag))
		{
			TArray<FHitResult> HitResults;
			if (GetWorld()->SweepMultiByChannel(HitResults, Character->GetActorLocation(), Character->GetActorLocation() + FVector::UpVector
				, FQuat::Identity, CollisionChannel, FCollisionShape::MakeSphere(WrapperSkill->Range)))
			{
				// TODO: better select target
				BlackBoard->SetValueAsInt(SpellSelector.SelectedKeyName, i + 1); // spells are 1 based
				BlackBoard->SetValueAsVector(SpellLocationSelector.SelectedKeyName, HitResults[0].ImpactPoint);
				BlackBoard->SetValueAsObject(SpellTargetSelector.SelectedKeyName, HitResults[0].GetActor());
			}
		}
	}
}

void UMMOBTService_SelectAbility::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	
}
