// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MMOAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BrainComponent.h"


AMMOAIController::AMMOAIController(const FObjectInitializer& ObjInit)
	: Super(ObjInit)
{
	SpellRequestKey = FName("SpellRequest");
	SpellActorTargetKey = FName("SpellActorTarget");
	SpellLocationTargetKey = FName("SpellLocationTarget");
}

void AMMOAIController::StopBehaviorTree(const FString& Reason)
{
	if (BrainComponent)
	{
		BrainComponent->StopLogic(Reason);
	}
}

void AMMOAIController::StartBehaviorTree(UBehaviorTree* NewBT)
{
	StopBehaviorTree(FString("Start New BT"));
	RunBehaviorTree(NewBT);
}

void AMMOAIController::RequestCastSpell(AActor* Target, const FVector& Location, int32 SpellIndex)
{
	if (SpellIndex < 0)
		return;

	if (UBlackboardComponent* BB = Blackboard.Get())
	{
		BB->SetValueAsInt(SpellRequestKey, SpellIndex + 1);
		BB->SetValueAsVector(SpellLocationTargetKey, Location);
		BB->SetValueAsObject(SpellActorTargetKey, Target);
	}
}

void AMMOAIController::AddPlayerHandledAbility(const int32 AbilityIndex)
{
	if (AbilityIndex >= 0)
	{
		PlayerHandledSpellIndex.AddUnique(AbilityIndex);
	}
}

void AMMOAIController::RemovePlayerHandledAbility(const int32 AbilityIndex)
{
	PlayerHandledSpellIndex.Remove(AbilityIndex);
}

bool AMMOAIController::IsSpellHandledByThePlayer(const int32 AbilityIndex) const
{
	return PlayerHandledSpellIndex.Contains(AbilityIndex);
}

bool AMMOAIController::HasPendingSpell() const
{
	if (UBlackboardComponent* BB = Blackboard.Get())
	{
		return BB->GetValueAsInt(SpellRequestKey) > 0;
	}
	return false;
}
