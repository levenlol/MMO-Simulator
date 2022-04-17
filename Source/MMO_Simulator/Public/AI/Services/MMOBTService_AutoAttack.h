// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "MMOBTService_AutoAttack.generated.h"

/**
 * Auto-Attack node. Pawn always attempts to auto-attack if in range.
 */

UCLASS()
class MMO_SIMULATOR_API UMMOBTService_AutoAttack : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
