// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "Core/MMOCommon.h"
#include "MMODataFinder.generated.h"

class UAnimSequenceBase;

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMOCharacterProgressionContainer
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly, Category = Progression)
	TArray<FMMOCharacterAttributes> AttributesIncreasePerLevel;
};

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMOCharacterProgressionDataTable : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category = Progression)
	int32 Strength = 0;

	UPROPERTY(EditDefaultsOnly, Category = Progression)
	int32 Constitution = 0;

	UPROPERTY(EditDefaultsOnly, Category = Progression)
	int32 Intellect = 0;

	UPROPERTY(EditDefaultsOnly, Category = Progression)
	int32 Dexterity = 0;
};

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMORaceProgressionDataTable : public FMMOCharacterProgressionDataTable
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category = Progression)
	EMMOCharacterRace Race;
};

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMOAnimationDataTable : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	FMMOWeaponTypeCouple WeaponsType;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimSequenceBase* WeaponAnimation;
};

UCLASS(config = game, defaultconfig)
class MMO_SIMULATOR_API UMMODataFinder : public UObject
{
	GENERATED_BODY()
public:
	static void Startup(UObject* Outer);
	static void Shutdown();

	UPROPERTY(config)
	FName AnimationDataTableName;

	UPROPERTY(config)
	FName RaceDataTableName;

	UPROPERTY(config)
	FString CharacterProgressionTableName;

	UFUNCTION(BlueprintPure, Category = CharacterProgression)
	FMMOCharacterAttributes GetCharacterProgressionForLevels(const EMMOCharacterClass CharacterClass, int32 StartLevel, int32 EndLevel) const;

	UFUNCTION(BlueprintPure, Category = CharacterProgression)
	FMMOCharacterAttributes GetRaceAttributes(const EMMOCharacterRace InRace) const;

	static UMMODataFinder* Get() { check(Instance); return Instance; }

	UAnimSequenceBase* GetAnimSequence(const FMMOWeaponTypeCouple& WeaponCouple) const;

private:
	static UMMODataFinder* Instance;

	UPROPERTY(VisibleAnywhere, Category = Animation)
	TMap<FMMOWeaponTypeCouple, UAnimSequenceBase*> AnimationsMap;

	UPROPERTY(VisibleAnywhere, Category = Animation)
	TMap<EMMOCharacterClass, FMMOCharacterProgressionContainer> AttributesIncreasePerLevel;

	UPROPERTY(VisibleAnywhere, Category = Animation)
	TMap<EMMOCharacterRace, FMMOCharacterAttributes> RaceAttributes;

	void Init();
	void Uninit();

	void ParseAnimationDataTable();
	void ParseRaceCharacterDataTable();
	void ParseCharacterProgressionDataTable();
};
