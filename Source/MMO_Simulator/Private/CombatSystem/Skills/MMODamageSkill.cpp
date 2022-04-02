// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystem/Skills/MMODamageSkill.h"
#include "Characters/MMOBaseCharacter.h"
#include "Core/MMOCommon.h"


void UMMODamageSkill::Setup(AMMOBaseCharacter* InOwner)
{
	Super::Setup(InOwner);

	DamageExpression.Init(this);

	if (FxActorClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = OwnerCharacter;
		FxActor = GetWorld()->SpawnActor<AMMOFXActor>(FxActorClass, OwnerCharacter->GetActorLocation(), OwnerCharacter->GetActorRotation(), SpawnParams);
		FxActor->SetActorHiddenInGame(true);
	}
}

void UMMODamageSkill::CastAbility(const FMMOSkillInputData& Data)
{
	if (AMMOBaseCharacter* Target = Cast<AMMOBaseCharacter>(Data.TargetActor))
	{
		// Compute and apply damage
		Target->DamageTake(ComputeDamage());

		// Play FX
		UMMOGameplayUtils::PlayParticlesAt(FxActor, Target->GetActorLocation());
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
