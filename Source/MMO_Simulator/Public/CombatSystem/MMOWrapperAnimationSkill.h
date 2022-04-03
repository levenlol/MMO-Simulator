// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatSystem/MMOWrapperSkill.h"
#include "MMOWrapperAnimationSkill.generated.h"

class UAnimationAsset;

// Wrapper animation that also play a custom animation.
UCLASS()
class MMO_SIMULATOR_API UMMOWrapperAnimationSkill : public UMMOWrapperSkill
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	UAnimationAsset* AnimationAsset;

private:
	virtual void TryCastAbility(const FMMOSkillInputData& Data) override;
	virtual void AbortAbility() override;
	virtual void FinishCastAbility() override;

	FTimerHandle AnimationHandle;

	UFUNCTION()
	void OnAnimationEnd();

	void StopAnimation();
};
