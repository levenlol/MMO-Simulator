// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystem/MMOAoeDamageSkill.h"

void UMMOAoeDamageSkill::CastAbility(FMMOSkillInputData Data)
{
	const FVector Location = Data.TargetActor ? Data.TargetActor->GetActorLocation() : Data.Location;
	//if (GetWorld()->SweepMultiByChannel())
	{

	}
}
