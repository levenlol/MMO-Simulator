// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "GameplayTagContainer.h"
#include "MMOBTDecorator_HasTag.generated.h"

UENUM(BlueprintType)
enum class EMMOTagCheckType : uint8
{
	HasAny,
	HasAnyExact,
	HasAll,
	HasAllExact
};

UCLASS()
class MMO_SIMULATOR_API UMMOBTDecorator_HasTag : public UBTDecorator
{
	GENERATED_BODY()
public:
	UMMOBTDecorator_HasTag();

	UPROPERTY(EditAnywhere, Category = Tag)
	FGameplayTagContainer Tags;

	UPROPERTY(EditAnywhere, Category = Tag)
	EMMOTagCheckType TagCheck;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual FString GetStaticDescription() const override;

};
