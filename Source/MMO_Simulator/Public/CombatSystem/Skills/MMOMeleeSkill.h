// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatSystem/Skills/MMOBaseSkill.h"
#include "MMOMeleeSkill.generated.h"

class UAnimationAsset;

UCLASS()
class MMO_SIMULATOR_API UMMOMeleeSkill : public UMMOBaseSkill
{
	GENERATED_BODY()
public:
	virtual void Setup(AMMOBaseCharacter* InOwner) override;

	virtual void CastAbility(const FMMOSkillInputData& Data) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	UAnimationAsset* AnimationAsset;
};
