// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatSystem/Skills/MMOBaseSkill.h"
#include "MMOCameraShakeSkill.generated.h"

class AMMOPlayerCameraManager;

UCLASS()
class MMO_SIMULATOR_API UMMOCameraShakeSkill : public UMMOBaseSkill
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill)
	TSubclassOf<UCameraShakeBase> CameraShake;

	virtual void Setup(AMMOBaseCharacter* InOwner) override;

	virtual void CastAbility(const FMMOSkillInputData& Data) override;
private:
	UPROPERTY()
	AMMOPlayerCameraManager* CameraManager = nullptr;
};
