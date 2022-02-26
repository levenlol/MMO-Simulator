// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystem/MMODamageSkill.h"
#include "Characters/MMOBaseCharacter.h"

void UMMODamageSkill::CastAbility(FMMOSkillInputData Data)
{
	if (AMMOBaseCharacter* Target = Cast<AMMOBaseCharacter>(Data.TargetActor))
	{
		Target->DamageTake(ComputeDamage());
	}
	else
	{
		// TODO: location.
		ensure(0);
	}
}

FMMODamage UMMODamageSkill::ComputeDamage() const
{
	return FMMODamage(FMath::RandRange(Damage.X, Damage.Y), DamageType, false, OwnerCharacter);
}
