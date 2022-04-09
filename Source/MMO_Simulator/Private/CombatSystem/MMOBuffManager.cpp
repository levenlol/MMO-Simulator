// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystem/MMOBuffManager.h"
#include "CombatSystem/Buffs/MMOBaseBuff.h"

UMMOBuffManager::UMMOBuffManager()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMMOBuffManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void UMMOBuffManager::ProcessBuffs(float DeltaTime)
{
	const int32 BuffsNum = Buffs.Num();
	for (int32 i = 0; i < BuffsNum; i++)
	{
		Buffs[i]->Tick(DeltaTime);
	}
}

void UMMOBuffManager::PurgeBuffs()
{
	const int32 BuffsNum = Buffs.Num();
	for (int32 i = BuffsNum - 1; i >= 0; i--)
	{
		if (RemainingBuffsTime[i] < 0.f)
		{
			Buffs[i]->Deactivate();

			Buffs.RemoveAt(i);
			RemainingBuffsTime.RemoveAt(i);
		}
	}
}

void UMMOBuffManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ProcessBuffs(DeltaTime);
	PurgeBuffs();
}

FMMODamage UMMOBuffManager::ProcessDamageDone(FMMODamage Damage)
{
	return Damage;
}

void UMMOBuffManager::AddBuff(UMMOBaseBuff* Buff, float Duration)
{
	if (Buff && Duration > 0.f)
	{
		Buff->Activate();

		Buffs.Add(Buff);
		RemainingBuffsTime.Add(Duration);
	}
}

