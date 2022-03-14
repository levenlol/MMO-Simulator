// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/MMOBTTask_CastAbility.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CombatSystem/MMOCombatSystem.h"
#include "Characters/MMOBaseCharacter.h"
#include "AIController.h"



UMMOBTTask_CastAbility::UMMOBTTask_CastAbility()
	: Super()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UMMOBTTask_CastAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIOwner = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackBoard = AIOwner->GetBlackboardComponent();

	AMMOBaseCharacter* Character = Cast<AMMOBaseCharacter>(AIOwner->GetPawn());

	if (BlackBoard && Character)
	{
		const int32 SpellIndex = BlackBoard->GetValueAsInt(SpellSelectorKey.SelectedKeyName) - 1;
		if (Character->CombatSystem->Skills.IsValidIndex(SpellIndex))
		{
			const FVector Location = BlackBoard->GetValueAsVector(TargetLocationSelectorKey.SelectedKeyName);
			AMMOBaseCharacter* Target = Cast<AMMOBaseCharacter>(BlackBoard->GetValueAsObject(TargetActorSelectorKey.SelectedKeyName));

			Character->CombatSystem->TryCastSkill(Target, Location, SpellIndex);

			return EBTNodeResult::InProgress;
		}
	}

	return EBTNodeResult::Failed;
}

void UMMOBTTask_CastAbility::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIOwner = OwnerComp.GetAIOwner();
	AMMOBaseCharacter* Character = Cast<AMMOBaseCharacter>(AIOwner->GetPawn());
	UBlackboardComponent* BlackBoard = AIOwner->GetBlackboardComponent();


	if(!Character || !Character->CombatSystem || !BlackBoard)
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to Complete Task: UMMOBTTask_CastAbility"));
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}

	//if (BlackBoard->IsValidKey(SpellSelectorKey.GetSelectedKeyID()))
	//{
	//	ClearBlackboardKeys(BlackBoard);
	//	return;
	//}

	if (!Character->CombatSystem->IsCasting())
	{
		ClearBlackboardKeys(BlackBoard);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

void UMMOBTTask_CastAbility::ClearBlackboardKeys(UBlackboardComponent* BlackBoard)
{
	BlackBoard->ClearValue(SpellSelectorKey.SelectedKeyName);
	BlackBoard->ClearValue(TargetLocationSelectorKey.SelectedKeyName);
	BlackBoard->ClearValue(TargetActorSelectorKey.SelectedKeyName);
}
