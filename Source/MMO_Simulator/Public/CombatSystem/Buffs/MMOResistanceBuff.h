// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatSystem/Buffs/MMOBaseBuff.h"
#include "Core//MMOCommon.h"
#include "MMOResistanceBuff.generated.h"

/**
 * 
 */
UCLASS()
class MMO_SIMULATOR_API UMMOResistanceBuff : public UMMOBaseBuff
{
	GENERATED_BODY()
public:
	// Percent damage to add based on damage type.
	UPROPERTY(EditAnywhere, Category = Debuff)
	TMap<EMMODamageType, int32> ResistanceMap;

	// usefull to modify damage taken (IE: Vulnerability to damage);
	void ProcessDamageTaken(FMMODamage& InOutDamage);
};
