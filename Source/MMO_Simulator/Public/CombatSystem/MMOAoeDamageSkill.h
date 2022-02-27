// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatSystem/MMODamageSkill.h"
#include "MMOAoeDamageSkill.generated.h"

class UParticleSystem;

UCLASS()
class MMO_SIMULATOR_API UMMOAoeDamageSkill : public UMMODamageSkill
{
	GENERATED_BODY()
public:
	UMMOAoeDamageSkill();
	virtual void CastAbility(FMMOSkillInputData Data) override;

	UPROPERTY(EditAnywhere, Category = Damage)
	TEnumAsByte<ECollisionChannel> CollisionChannel;

	UPROPERTY(EditAnywhere, Category = FX)
	UParticleSystem* ParticleToPlay = nullptr;

	// cm. Used to scale particle based on damage radius. Scale = Radius / ParticlesSize.
	UPROPERTY(EditAnywhere, Category = FX, meta = (ClampMin = "1.0"))
	float ParticlesSize = 100.f;

	UPROPERTY(EditAnywhere, Category = Damage, meta=(ClampMin="1.0"))
	float Radius = 100.f;
 };
