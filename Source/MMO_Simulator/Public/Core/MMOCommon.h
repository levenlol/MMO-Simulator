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
struct MMO_SIMULATOR_API FCharacterAttributes
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	ECharacterClass CharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	int32 Strength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	int32 Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	int32 Intellect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	int32 Dexterity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	int32 Happiness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	int32 Greediness;
};

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FCharacterStats
{
	GENERATED_BODY()
public:
	// Max Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats, meta = (ClampMin = "1"))
	int32 MaxHealth = 1;
	
	//Current Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int32 Health;

	// Max resources
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats, meta = (ClampMin = "1"))
	int32 MaxResources = 1;

	// Mana, rage, energy...
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int32 Resources;

	// Health per seconds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int32 HPS;

	// Resources per seconds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int32 RPS;

	// Recuperate Health and Resource every X seconds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats, meta = (ClampMin = "0"))
	int32 RecuperateEverySeconds = 1;

	void Recuperate(int32 ElapsedSeconds);
};

