// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatSystem/Buffs/MMOBaseBuff.h"
#include "Tools/MMOMathExpression.h"
#include "Core/MMOCommon.h"
#include "MMOHotBuff.generated.h"

/**
 * 
 */
UCLASS()
class MMO_SIMULATOR_API UMMOHotBuff : public UMMOBaseBuff
{
	GENERATED_BODY()
public:
	// DamageExpression, Root is this debuff.
	UPROPERTY(EditDefaultsOnly, Category = Damage)
	FMMOMathExpression HealingExpression;

	virtual void Init(AMMOBaseCharacter* InOwner) override;
	virtual void Activate() override;
	virtual void ReceiveExecute() override;
	virtual void Deactivate() override;
	virtual void Uninit() override;
private:
	UPROPERTY(VisibleInstanceOnly, Category = Damage)
	FMMODamage HealingPerTick;
};
