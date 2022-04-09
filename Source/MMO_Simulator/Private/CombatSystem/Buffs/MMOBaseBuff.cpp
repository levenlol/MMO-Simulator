// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystem/Buffs/MMOBaseBuff.h"

UMMOBaseBuff::UMMOBaseBuff()
	: Super()
{
}

void UMMOBaseBuff::Init(AMMOBaseCharacter* Owner)
{
	OwnerCharacter = Owner;
	AccDeltaTime = 0.f;
}

void UMMOBaseBuff::Activate()
{
	AccDeltaTime = 0.f;
}

void UMMOBaseBuff::Tick(float DeltaTime)
{
	AccDeltaTime += DeltaTime;
	if (AccDeltaTime >= TickTime)
	{
		ReceiveExecute();
		AccDeltaTime -= TickTime;
	}
}
