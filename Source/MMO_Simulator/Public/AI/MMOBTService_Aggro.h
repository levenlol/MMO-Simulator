// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "MMOBTService_Aggro.generated.h"

/**
 * 
 */
UCLASS()
class MMO_SIMULATOR_API UMMOBTService_Aggro : public UBTService_BlackboardBase
{
	GENERATED_BODY()
public:
	UMMOBTService_Aggro();
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
