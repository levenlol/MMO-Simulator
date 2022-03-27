// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "MMOBTDecorator_IsNear.generated.h"

/**
 * A version of IsAtLocation that allows interruption of children.
 */
UCLASS()
class MMO_SIMULATOR_API UMMOBTDecorator_IsNear : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()
public:
	UMMOBTDecorator_IsNear();

	UPROPERTY(EditAnywhere, Category = Default, meta=(ClampMin="1.0"))
	float AcceptableRadius = 50.f;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
