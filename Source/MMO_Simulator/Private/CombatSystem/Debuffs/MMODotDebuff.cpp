// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystem/Debuffs/MMODotDebuff.h"
#include "Characters/MMOBaseCharacter.h" 

void UMMODotDebuff::Init(AMMOBaseCharacter* InOwner)
{
	Super::Init(InOwner);

}

void UMMODotDebuff::Activate(AMMOBaseCharacter* InTarget)
{
	Super::Activate(InTarget);

	DamageExpression.Init(this);
	DamageExpression.RefreshVariables();

	if (DamageExpression.IsValid())
	{
		DamagePerTick.bCrit = false;
		DamagePerTick.DamageDealer = OwnerCharacter;
		DamagePerTick.DamageType = DamageType;
		DamagePerTick.Damage = DamageExpression.Eval<int32>();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Debuff %s have a invalid damage expression"), *GetName());
	}
}

void UMMODotDebuff::ReceiveExecute()
{
	Super::ReceiveExecute();

	if (TargetCharacter && OwnerCharacter)
	{
		OwnerCharacter->DoDamage(DamagePerTick, TargetCharacter);
	}
}

void UMMODotDebuff::Deactivate()
{
}

void UMMODotDebuff::Uninit()
{
	Super::Uninit();

	DamageExpression.Release();
}
