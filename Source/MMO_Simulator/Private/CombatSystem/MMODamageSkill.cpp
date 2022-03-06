// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystem/MMODamageSkill.h"
#include "Characters/MMOBaseCharacter.h"

void UMMODamageSkill::Setup(AMMOBaseCharacter* InOwner)
{
	Super::Setup(InOwner);

	DamageExpression.Init(this);
}

void UMMODamageSkill::CastAbility(FMMOSkillInputData Data)
{
	if (AMMOBaseCharacter* Target = Cast<AMMOBaseCharacter>(Data.TargetActor))
	{
		Target->DamageTake(ComputeDamage());
	}
	else
	{
		const FString SkillName = *GetOuterSkill()->GetName();
		UE_LOG(LogTemp, Warning, TEXT("Skill: %s cannot do damage to location, expected a target."), *SkillName);
	}
}

FMMODamage UMMODamageSkill::ComputeDamage()
{
	int32 TotalDamage = FMath::RandRange(Damage.X, Damage.Y);
	if (DamageExpression.IsValid())
	{
		Dmg = static_cast<double>(TotalDamage);

		TotalDamage = DamageExpression.Eval<int32>();
	}

	return FMMODamage(TotalDamage, DamageType, false, OwnerCharacter);
}
