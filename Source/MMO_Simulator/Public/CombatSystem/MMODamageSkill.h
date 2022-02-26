// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatSystem/MMOBaseSkill.h"
#include "Core/MMOCommon.h"

#include "MMODamageSkill.generated.h"

UCLASS()
class MMO_SIMULATOR_API UMMODamageSkill : public UMMOBaseSkill
{
	GENERATED_BODY()
public:
	virtual void CastAbility(FMMOSkillInputData Data) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Damage)
	FIntPoint Damage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Damage)
	EMMODamageType DamageType;

	FMMODamage ComputeDamage() const;
};
