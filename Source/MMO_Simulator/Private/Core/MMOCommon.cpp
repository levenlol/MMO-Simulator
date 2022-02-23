// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MMOCommon.h"
#include "Data/MMODataFinder.h"

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

UAnimSequenceBase* UMMOGameplayUtils::GetAnimSequence(const FMMOWeaponTypeCouple& WeaponCouple)
{
	UMMODataFinder* DataFinder = UMMODataFinder::Get();
	return DataFinder->GetAnimSequence(WeaponCouple);
}

float UMMOGameplayUtils::PlayRateToFitAnimation(const UAnimSequenceBase* Anim, float TargetSeconds)
{
	if (Anim && !FMath::IsNearlyZero(TargetSeconds, KINDA_SMALL_NUMBER))
	{
		return Anim->GetPlayLength() / TargetSeconds;
	}

	UE_LOG(LogTemp, Error, TEXT("Cannot Fit Animation %s to seconds %f"), Anim ? *Anim->GetName() : TEXT("NULL"), TargetSeconds);
	return 1.0f;
}
