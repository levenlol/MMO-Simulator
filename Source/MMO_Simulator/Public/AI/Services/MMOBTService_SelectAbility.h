// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "MMOBTService_SelectAbility.generated.h"

/**
 * 
 */
UCLASS()
class MMO_SIMULATOR_API UMMOBTService_SelectAbility : public UBTService
{
	GENERATED_BODY()
public:
	UMMOBTService_SelectAbility();
	
	UPROPERTY(EditAnywhere, Category = Spell)
	FBlackboardKeySelector SpellSelector;

	UPROPERTY(EditAnywhere, Category = Spell)
	FBlackboardKeySelector SpellLocationSelector;

	UPROPERTY(EditAnywhere, Category = Spell)
	FBlackboardKeySelector SpellTargetSelector;

	UPROPERTY(EditAnywhere, Category = Spell)
	TEnumAsByte<ECollisionChannel> CollisionChannel;

	virtual void OnSearchStart(FBehaviorTreeSearchData& SearchData) override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
