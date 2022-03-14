// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MMOAIController.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "BehaviorTree/BlackboardComponent.h"


AMMOAIController::AMMOAIController(const FObjectInitializer& ObjInit)
	: Super(ObjInit.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	SpellRequestKey = FName("SpellRequest");
	SpellActorTargetKey = FName("SpellActorTarget");
	SpellLocationTargetKey = FName("SpellLocationTarget");
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

bool AMMOAIController::HasPendingSpell() const
{
	if (UBlackboardComponent* BB = Blackboard.Get())
	{
		return BB->GetValueAsInt(SpellRequestKey) > 0;
	}
	return false;
}
