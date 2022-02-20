// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MMOBTTask_WaitAttackDelay.h"
#include "AIController.h"
#include "Characters/MMOBaseCharacter.h"

UMMOBTTask_WaitAttackDelay::UMMOBTTask_WaitAttackDelay(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "WaitAttackDelay";
	WaitTime = 3.0f;
	INIT_TASK_NODE_NOTIFY_FLAGS();
}

EBTNodeResult::Type UMMOBTTask_WaitAttackDelay::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FMMOWaitAttackDelayMemory* MyMemory = CastInstanceNodeMemory<FMMOWaitAttackDelayMemory>(NodeMemory);

	AAIController* AIOwner = OwnerComp.GetAIOwner();
	if (AMMOBaseCharacter* Character = Cast<AMMOBaseCharacter>(AIOwner->GetPawn()))
	{
		MyMemory->RemainingWaitTime = WaitTime;

		return EBTNodeResult::InProgress;
	}

	UE_LOG(LogTemp, Error, TEXT("Unable to ExecuteTask: MMOBTTask_WaitAttackDelay"));
	return EBTNodeResult::Failed;
}

void UMMOBTTask_WaitAttackDelay::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FMMOWaitAttackDelayMemory* MyMemory = CastInstanceNodeMemory<FMMOWaitAttackDelayMemory>(NodeMemory);
	MyMemory->RemainingWaitTime -= DeltaSeconds;

	if (MyMemory->RemainingWaitTime <= 0.0f)
	{
		// continue execution from this node
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

void UMMOBTTask_WaitAttackDelay::DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const
{
	Super::DescribeRuntimeValues(OwnerComp, NodeMemory, Verbosity, Values);

	FMMOWaitAttackDelayMemory* MyMemory = CastInstanceNodeMemory<FMMOWaitAttackDelayMemory>(NodeMemory);
	if (MyMemory->RemainingWaitTime)
	{
		Values.Add(FString::Printf(TEXT("remaining: %ss"), *FString::SanitizeFloat(MyMemory->RemainingWaitTime)));
	}
}

uint16 UMMOBTTask_WaitAttackDelay::GetInstanceMemorySize() const
{
	return sizeof(FMMOWaitAttackDelayMemory);
}

#if WITH_EDITOR

FName UMMOBTTask_WaitAttackDelay::GetNodeIconName() const
{
	return FName("BTEditor.Graph.BTNode.Task.Wait.Icon");
}

#endif	// WITH_EDITOR
