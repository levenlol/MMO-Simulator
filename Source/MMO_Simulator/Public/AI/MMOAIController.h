// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MMOAIController.generated.h"

/**
 * 
 */
UCLASS()
class MMO_SIMULATOR_API AMMOAIController : public AAIController
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = Spell)
	void RequestCastSpell(int32 SpellIndex);

	UFUNCTION(BlueprintPure, Category = Spell)
	bool HasPendingSpell() const { return PendingSpell >= 0; }
private:
	UPROPERTY(VisibleAnywhere, Category = Spell)
	int32 PendingSpell;
};
