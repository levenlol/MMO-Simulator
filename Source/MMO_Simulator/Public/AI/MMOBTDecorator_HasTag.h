// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "GameplayTagContainer.h"
#include "MMOBTDecorator_HasTag.generated.h"

/**
 * 
 */
UCLASS()
class MMO_SIMULATOR_API UMMOBTDecorator_HasTag : public UBTDecorator
{
	GENERATED_BODY()
public:
	UMMOBTDecorator_HasTag();
	UPROPERTY(EditAnywhere, Category = Tag)
	FGameplayTag Tag;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual FString GetStaticDescription() const override;

};
