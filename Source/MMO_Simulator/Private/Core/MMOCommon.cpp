// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MMOCommon.h"

void FMMOCharacterStats::Recuperate(int32 ElapsedSeconds)
{
	Health = FMath::Clamp(Health + ElapsedSeconds * HPS, 0, MaxHealth);
	Resources = FMath::Clamp(Resources + ElapsedSeconds * RPS, 0, MaxResources);
}

bool UMMOGameplayUtils::Is2HWeapon(EMMOWeaponType WeaponType)
{
	switch (WeaponType)
	{
	case EMMOWeaponType::Axe2h:
	case EMMOWeaponType::Sword2h:
	case EMMOWeaponType::Mace2h:
	case EMMOWeaponType::Bow:
	case EMMOWeaponType::Rifle:
	case EMMOWeaponType::Spear:
	case EMMOWeaponType::Staff:
		return true;
	default:
		break;
	}

	return false;
}
