// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatSystem/Skills/MMOBaseSkill.h"
#include "Core/MMOCommon.h"
#include "Tools/MMOMathExpression.h"
#include "MMODamageSkill.generated.h"

UCLASS()
class MMO_SIMULATOR_API UMMODamageSkill : public UMMOBaseSkill
{
	GENERATED_BODY()
public:
	virtual void Setup(AMMOBaseCharacter* InOwner) override;

	virtual void CastAbility(FMMOSkillInputData Data) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Damage)
	FIntPoint Damage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Damage)
	EMMODamageType DamageType;

	// DamageExpression, Root is this skill. Use Dmg to reference the damage computed by random (Damange.X, Damage.Y). OwnerCharacter to reference the owner.
	// IE: if we want the formula to be: TotalDamage = Rand(Damage.X, Damage.Y) + Owner.Strength we do: Dmg + OwnerCharacter.CharacterInfo.Attributes.Strength
	UPROPERTY(EditDefaultsOnly, Category = Damage)
	FMMOMathExpression DamageExpression;

protected:
	UPROPERTY(EditDefaultsOnly, Category = FX)
	TSubclassOf<AMMOFXActor> FxActorClass;

	UPROPERTY(VisibleAnywhere, Category = FX)
	AMMOFXActor* FxActor;

	FMMODamage ComputeDamage();

private:
	UPROPERTY(VisibleAnywhere, Category = Damage)
	double Dmg = 0.0;
};
