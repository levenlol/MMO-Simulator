// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatSystem/Debuffs/MMOBaseDebuff.h"
#include "Tools/MMOMathExpression.h"
#include "Core/MMOCommon.h"
#include "Characters/MMOBaseCharacter.h"

#include "MMODotDebuff.generated.h"

class AMMOBaseCharacter;

UCLASS()
class MMO_SIMULATOR_API UMMODotDebuff : public UMMOBaseDebuff
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Damage)
	EMMODamageType DamageType;

	// DamageExpression, Root is this debuff.
	UPROPERTY(EditDefaultsOnly, Category = Damage)
	FMMOMathExpression DamageExpression;

	virtual void Init(AMMOBaseCharacter* InOwner) override;
	virtual void Activate(AMMOBaseCharacter* InTarget) override; 
	virtual void ReceiveExecute() override;
	virtual void Deactivate() override;
	virtual void Uninit() override;
private:
	UPROPERTY(VisibleInstanceOnly, Category = Damage)
	FMMODamage DamagePerTick;
};
