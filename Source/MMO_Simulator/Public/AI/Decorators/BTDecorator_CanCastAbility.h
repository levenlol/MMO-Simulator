// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_CanCastAbility.generated.h"

/**
 * 
 */
UCLASS()
class MMO_SIMULATOR_API UBTDecorator_CanCastAbility : public UBTDecorator
{
	GENERATED_BODY()
public:
	UBTDecorator_CanCastAbility();

	UPROPERTY(EditAnywhere, Category = Skill)
	FBlackboardKeySelector SpellRequestedSelectorKey;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
