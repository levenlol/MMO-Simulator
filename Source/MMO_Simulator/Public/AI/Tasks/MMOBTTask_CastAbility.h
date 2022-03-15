// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MMOBTTask_CastAbility.generated.h"

class UBlackboardComponent;

UCLASS()
class MMO_SIMULATOR_API UMMOBTTask_CastAbility : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UMMOBTTask_CastAbility();

	UPROPERTY(EditAnywhere, Category = Skill)
	FBlackboardKeySelector SpellSelectorKey;

	UPROPERTY(EditAnywhere, Category = Skill)
	FBlackboardKeySelector TargetLocationSelectorKey;

	UPROPERTY(EditAnywhere, Category = Skill)
	FBlackboardKeySelector TargetActorSelectorKey;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	void ClearBlackboardKeys(UBlackboardComponent* BlackBoard);
};
