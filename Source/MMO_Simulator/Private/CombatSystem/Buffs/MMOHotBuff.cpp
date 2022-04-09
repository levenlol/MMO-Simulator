// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystem/Buffs/MMOHotBuff.h"
#include "Characters/MMOBaseCharacter.h"

void UMMOHotBuff::Init(AMMOBaseCharacter* InOwner)
{
	Super::Init(InOwner);

	HealingExpression.Init(this);
}

void UMMOHotBuff::Activate()
{
	Super::Activate();

	HealingPerTick.bCrit = false;
	HealingPerTick.Damage = -HealingExpression.Eval<int32>(true);
	HealingPerTick.DamageDealer = OwnerCharacter;
	HealingPerTick.DamageType = EMMODamageType::Holy;
}

void UMMOHotBuff::ReceiveExecute()
{
	Super::ReceiveExecute();

	if (OwnerCharacter)
	{
		OwnerCharacter->DoDamage(HealingPerTick, OwnerCharacter);
	}
}

void UMMOHotBuff::Deactivate()
{
	Super::Deactivate();
}

void UMMOHotBuff::Uninit()
{
	Super::Uninit();

	HealingExpression.Release();
}
