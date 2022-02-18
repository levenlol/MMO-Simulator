// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MMOCommon.h"

void FCharacterStats::Recuperate(int32 ElapsedSeconds)
{
	Health = FMath::Clamp(ElapsedSeconds * HPS, 0, MaxHealth);
	Resources = FMath::Clamp(ElapsedSeconds * RPS, 0, MaxHealth);
}
