// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatSystem/Skills/MMOBaseSkill.h"
#include "MMOTimeTickSkill.generated.h"

/**
 * 
 */
UCLASS()
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
	virtual void CastAbility(FMMOSkillInputData Data) override;

	virtual void StartTick() {}
	virtual void EndTick() {}

	virtual void Step(int32 TickCount) {}

	UFUNCTION()
	void Tick();
private:
	FTimerHandle TimerHandle;
	int32 CurrentTick = 0;
};
