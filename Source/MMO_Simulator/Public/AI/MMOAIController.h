// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "MMOAIController.generated.h"

/**
 * 
 */
UCLASS()
class MMO_SIMULATOR_API AMMOAIController : public AAIController
{
	GENERATED_BODY()
public:
	AMMOAIController(const FObjectInitializer& ObjInit);

	UFUNCTION(BlueprintCallable, Category = Spell)
	void RequestCastSpell(AActor* Target, const FVector& Location, int32 SpellIndex);

	UFUNCTION(BlueprintPure, Category = Spell)
	bool HasPendingSpell() const;
private:

	UPROPERTY(EditAnywhere, Category = Spell, meta=(AllowPrivateAccess))
	FName SpellRequestKey;

	UPROPERTY(EditAnywhere, Category = Spell, meta = (AllowPrivateAccess))
	FName SpellActorTargetKey;

	UPROPERTY(EditAnywhere, Category = Spell, meta = (AllowPrivateAccess))
	FName SpellLocationTargetKey;
};
