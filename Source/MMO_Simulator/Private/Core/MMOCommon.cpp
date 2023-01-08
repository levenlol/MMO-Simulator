// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MMOCommon.h"
#include "Data/MMODataFinder.h"
#include "Items/MMOBaseWeapon.h"
#include "Characters/MMOBaseCharacter.h"
#include "Characters/MMOBaseEnemy.h"
#include "Characters/MMOBaseHero.h"
#include "Core/MMOGameState.h"
#include "Components/Button.h"
#include "Engine/StreamableManager.h"
#include "Data/MMODataFinder.h"
#include "Kismet/GameplayStatics.h"
#include "CombatSystem/Skills/MMOBaseSkill.h"
#include "Particles/ParticleSystemComponent.h"


void FMMOCharacterStats::Recuperate(int32 ElapsedSeconds)
{
	Health = FMath::Clamp(Health + ElapsedSeconds * HPS, 0, MaxHealth);
	Resources = FMath::Clamp(Resources + ElapsedSeconds * RPS, 0, MaxResources);
}

bool FMMOCharacterStats::RequestResource(int32 Amount)
{
	if (Resources >= Amount)
	{
		Resources -= Amount;
		return true;
	}
	return false;
}

FMMOCharacter::FMMOCharacter(const FName& InName)
	: Name(InName)
{}
