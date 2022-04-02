// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatSystem/Skills/MMODamageSkill.h"
#include "MMOAoeDamageSkill.generated.h"

UCLASS()
class MMO_SIMULATOR_API UMMOAoeDamageSkill : public UMMODamageSkill
{
	GENERATED_BODY()
public:
	UMMOAoeDamageSkill();
	virtual void Setup(AMMOBaseCharacter* InOwner) override;
	virtual void CastAbility(const FMMOSkillInputData& Data) override;

	UPROPERTY(EditAnywhere, Category = Damage)
	TEnumAsByte<ECollisionChannel> CollisionChannel;

	UPROPERTY(EditAnywhere, Category = Damage, meta=(ClampMin="1.0"))
	float Radius = 100.f;
 };
