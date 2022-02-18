// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MMOCommon.generated.h"

UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
	Warrior,
	Paladin,
	Berserker,
	DeathKnight,
	Shaman,
	Monk,
	Priest,
	Druid,
	Mage,
	Warlock
};

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FCharacterStats
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	ECharacterClass CharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int32 Strength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int32 Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int32 Intellect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int32 Dexterity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int32 Happiness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int32 Greediness;
};