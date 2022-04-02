// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatSystem/Skills/MMOBaseSkill.h"
#include "MMOTimeTickSkill.generated.h"

// Skill-helper that wrap tick mechanics.
UCLASS(HideDropdown, NotBlueprintable)
class MMO_SIMULATOR_API UMMOTimeTickSkill : public UMMOBaseSkill
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
	float Duration = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup, meta=(ClampMin="1"))
	int32 TickNumber = 5;
protected:

	virtual void Setup(AMMOBaseCharacter* InOwner) override;
	virtual void CastAbility(const FMMOSkillInputData& Data) override;

	virtual void StartTick(const FMMOSkillInputData& Data) {}
	virtual void EndTick(const FMMOSkillInputData& Data) {}

	virtual void Step(const FMMOSkillInputData& Data, int32 TickCount) {}

	UFUNCTION()
	void Tick();
private:
	FTimerHandle TimerHandle;
	int32 CurrentTick = 0;

	FMMOSkillInputData InputData;
};

// Useful to make periodic damage tick.
UCLASS(Blueprintable)
class MMO_SIMULATOR_API UMMOTickSkill : public UMMOTimeTickSkill
{
	GENERATED_BODY()
public:
	virtual void Step(const FMMOSkillInputData& Data, int32 TickCount);
};
