// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatSystem/MMODamageSkill.h"
#include "MMOAoeDamageSkill.generated.h"

/**
 * 
 */
UCLASS()
class MMO_SIMULATOR_API UMMOAoeDamageSkill : public UMMODamageSkill
{
	GENERATED_BODY()
public:
	virtual void CastAbility(FMMOSkillInputData Data) override;


 };
