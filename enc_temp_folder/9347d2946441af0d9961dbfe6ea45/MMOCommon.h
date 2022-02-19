// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MMOCommon.generated.h"

UENUM(BlueprintType)
enum class EMMOCharacterClass : uint8
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

UENUM(BlueprintType)
enum class EMMODamageType : uint8
{
	Physical,
	Fire,
	Ice,
	Shadow,
	Holy,
	Nature,
	Arcane
};

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMOCharacterAttributes
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	EMMOCharacterClass CharacterClass;

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
struct MMO_SIMULATOR_API FMMOCharacterStats
{
	GENERATED_BODY()
public:
	// Max Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats, meta = (ClampMin = "1"))
	int32 MaxHealth = 100;
	
	//Current Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int32 Health = 70;

	// Max resources
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats, meta = (ClampMin = "1"))
	int32 MaxResources = 70;

	// Mana, rage, energy...
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int32 Resources = 30;

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

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMOResistances
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Resistance)
	int32 Armor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Resistance)
	int32 FireResistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Resistance)
	int32 IceResistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Resistance)
	int32 ShadowResistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Resistance)
	int32 HolyResistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Resistance)
	int32 NatureResistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Resistance)
	int32 ArcaneResistance;
};


USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMODamage
{
	GENERATED_BODY()
public:
	FMMODamage() = default;

	FMMODamage(int32 InDamage, EMMODamageType InDamageType, bool bInCrit)
		: Damage(InDamage)
		, DamageType(InDamageType)
		, bCrit(bInCrit) 
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
	int32 Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
	EMMODamageType DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
	bool bCrit = false;
};