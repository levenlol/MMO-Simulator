// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystem/MMODebuffManager.h"
#include "CombatSystem/Debuffs/MMOBaseDebuff.h"

UMMODebuffManager::UMMODebuffManager()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMMODebuffManager::BeginPlay()
{
	Super::BeginPlay();
}

void UMMODebuffManager::ProcessActiveDebuffs(float DeltaTime)
{
	const int32 DebuffsNum = Debuffs.Num();
	for (int32 i = 0; i < DebuffsNum; i++)
	{
		Debuffs[i]->Tick(DeltaTime);
		DebuffsRemainingDuration[i] = DebuffsRemainingDuration[i] - DeltaTime;
	}
}

void UMMODebuffManager::PurgeElapsedDebuffs()
{
	const int32 DebuffsNum = Debuffs.Num();
	for (int32 i = DebuffsNum - 1; i >= 0; i--)
	{
		if (DebuffsRemainingDuration[i] < 0.f)
		{
			Debuffs[i]->Deactivate();

			Debuffs.RemoveAt(i);
			DebuffsRemainingDuration.RemoveAt(i);
		}
	}
}

void UMMODebuffManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ProcessActiveDebuffs(DeltaTime);
	PurgeElapsedDebuffs();
}

void UMMODebuffManager::AddDebuff(UMMOBaseDebuff* Debuff, float Duration)
{
	if (Debuff && Duration > 0.f)
	{
		Debuffs.Add(Debuff);
		DebuffsRemainingDuration.Add(Duration);

		Debuff->Activate(GetOwnerCharacter());
	}
}

