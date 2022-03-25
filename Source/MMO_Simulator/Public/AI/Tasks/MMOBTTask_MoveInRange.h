// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "MMOBTTask_MoveInRange.generated.h"

/**
 * 
 */
UCLASS()
class MMO_SIMULATOR_API UMMOBTTask_MoveInRange : public UBTTask_MoveTo
{
	GENERATED_BODY()
public:
	UMMOBTTask_MoveInRange();

	// Used if character is unarmed.
	UPROPERTY(Category = Default, EditAnywhere, meta=(ClampMin = "0.0", UIMin="0.0"))
	float DefaultAcceptableRadius = 25.f;

	// Margin for Acceptable Radius. 0.1 means 10percent. AcceptableRadius = (1 - MarginPercent)WeaponRange
	UPROPERTY(Category = Default, EditAnywhere, meta = (ClampMin = "0.0", UIMin = "0.0", ClampMax = "1.0", UIMax = "1.0"))
	float MarginPercent = 0.1f;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
