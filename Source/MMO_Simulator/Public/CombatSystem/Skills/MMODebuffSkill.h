// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatSystem/Skills/MMOBaseSkill.h"
#include "MMODebuffSkill.generated.h"

class UMMOBaseDebuff;

UCLASS()
class MMO_SIMULATOR_API UMMODebuffSkill : public UMMOBaseSkill
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = Debuff)
	TSubclassOf<UMMOBaseDebuff> DebuffClass;

	UPROPERTY(EditAnywhere, Category = Debuff, meta=(ClampMin="1.0"))
	float Duration = 1.f;

	virtual void Setup(AMMOBaseCharacter* InOwner) override;
	virtual void CastAbility(const FMMOSkillInputData& Data) override;
private:
	UPROPERTY()
	UMMOBaseDebuff* Debuff = nullptr;
};
