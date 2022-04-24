// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatSystem/Skills/MMOBaseSkill.h"
#include "MMOTauntSkill.generated.h"

/**
 * 
 */
UCLASS()
class MMO_SIMULATOR_API UMMOTauntSkill : public UMMOBaseSkill
{
	GENERATED_BODY()
public:
	virtual void CastAbility(const FMMOSkillInputData& Data) override;
};
