// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "MMOBTDecorator_IsCasting.generated.h"

/**
 * 
 */
UCLASS()
class MMO_SIMULATOR_API UMMOBTDecorator_IsCasting : public UBTDecorator
{
	GENERATED_BODY()
public:
	UMMOBTDecorator_IsCasting();

	UPROPERTY(EditAnywhere, Category = Skill)
	FBlackboardKeySelector SpellRequestedSelectorKey;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
