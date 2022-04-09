// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystem/Debuffs/MMOBaseDebuff.h"

UMMOBaseDebuff::UMMOBaseDebuff()
	: Super()
{
}

void UMMOBaseDebuff::Init(AMMOBaseCharacter* InOwner)
{
	OwnerCharacter = InOwner;
	AccDeltaTime = 0.f;
}

void UMMOBaseDebuff::Activate(AMMOBaseCharacter* InTarget)
{
	TargetCharacter = InTarget;
	AccDeltaTime = 0.f;
}

void UMMOBaseDebuff::ReceiveExecute()
{
}

void UMMOBaseDebuff::Deactivate()
{
}

void UMMOBaseDebuff::Tick(float DeltaTime)
{
	AccDeltaTime += DeltaTime;
	if (AccDeltaTime >= TickTime)
	{
		ReceiveExecute();
		AccDeltaTime -= TickTime;
	}
}

void UMMOBaseDebuff::Uninit()
{
	OwnerCharacter = nullptr;
}
