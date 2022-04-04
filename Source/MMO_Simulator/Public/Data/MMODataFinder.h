// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "Core/MMOCommon.h"
#include "MMODataFinder.generated.h"

class UAnimSequenceBase;

enum class EMMORarityType : uint8;
enum class EMMOArmorType : uint8;
enum class EMMOArmorSlotType : uint8;
enum class EMMOWeaponType : uint8;

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMOEquipGeneratorDataTable : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	int32 Strength;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	int32 Constitution;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	int32 Intellect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	int32 Dexterity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	int32 HPS;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	int32 MPS;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	int32 MeleeCritRating;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	int32 SpellCritRating;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	int32 DodgeRating;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	int32 BlockRating;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	int32 AttackPower;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	int32 SpellPower;
};

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMOEquipGeneratorArmorTypeMultiplierDataTable : public FMMOEquipGeneratorDataTable
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	EMMOArmorType ArmorType;
};

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMOEquipGeneratorArmorSlotMultiplierDataTable : public FMMOEquipGeneratorDataTable
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	EMMOArmorSlotType ArmorSlot;
};

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMOEquipGeneratorWeaponTypeMultiplierDataTable : public FMMOEquipGeneratorDataTable
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	EMMOWeaponType WeaponType;
};

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMOEquipGeneratorQualifyingNameultiplierDataTable : public FMMOEquipGeneratorDataTable
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	FString QualifyingName;
};


USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FEquipGeneratorData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	TArray<FMMOEquipGeneratorDataTable> Data;

	const FMMOEquipGeneratorDataTable& GetEquipGeneratorForItemLevel(int32 ItemLevel) const;
};

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

	// Weapons animation
	UPROPERTY(config)
	FName AnimationDataTableName;

	// Race data table
	UPROPERTY(config)
	FName RaceDataTableName;

	// Equip Generator Keys
	UPROPERTY(config)
	FString EquipGenDataTableName;

	UPROPERTY(config)
	FName ArmorTypeDataTableName;

	UPROPERTY(config)
	FName ArmorSlotDataTableName;

	UPROPERTY(config)
	FName WeaponTypeDataTableName;

	UPROPERTY(config)
	FName QualifyingNameDataTableName;

	UPROPERTY(config)
	FString CharacterProgressionTableName;

	UFUNCTION(BlueprintPure, Category = CharacterProgression)
	FMMOCharacterAttributes GetCharacterProgressionAtLevel(const EMMOCharacterClass CharacterClass, int32 Level) const;

	UFUNCTION(BlueprintPure, Category = CharacterProgression)
	FMMOCharacterAttributes GetRaceAttributes(const EMMOCharacterRace InRace) const;

	static UMMODataFinder* Get() { check(Instance); return Instance; }

	UAnimSequenceBase* GetAnimSequence(const FMMOWeaponTypeCouple& WeaponCouple) const;

	const FMMOEquipGeneratorDataTable& GetEquipGeneratorData(EMMORarityType RarityType, int32 ItemLevel) const;
	const FMMOEquipGeneratorDataTable& GetEquipGeneratorArmorTypeMultipliers(EMMOArmorType ArmorType) const;
	const FMMOEquipGeneratorDataTable& GetEquipGeneratorArmorSlotMultipliers(EMMOArmorSlotType ArmorSlot) const;
	const FMMOEquipGeneratorDataTable& GetEquipGeneratorWeaponTypeMultipliers(EMMOWeaponType WeaponType) const;
	const FMMOEquipGeneratorQualifyingNameultiplierDataTable& GetEquipGeneratorRandomQualifyingMultipliers() const;
	const FMMOEquipGeneratorQualifyingNameultiplierDataTable& GetEquipGeneratorQualifyingMultipliers(const FString& Qualifier) const;


private:
	static UMMODataFinder* Instance;

	UPROPERTY(VisibleAnywhere, Category = Animation)
	TMap<FMMOWeaponTypeCouple, UAnimSequenceBase*> AnimationsMap;

	UPROPERTY(VisibleAnywhere, Category = Animation)
	TMap<EMMOCharacterClass, FMMOCharacterProgressionContainer> AttributesIncreasePerLevel;

	UPROPERTY(VisibleAnywhere, Category = Animation)
	TMap<EMMOCharacterRace, FMMOCharacterAttributes> RaceAttributes;

	void AddEquipGeneratorData(EMMORarityType Rarity, const FName& DataTableID);

	TMap<EMMORarityType, FEquipGeneratorData> EquipGeneratorItemLevel;
	TMap<EMMOArmorType, FEquipGeneratorData> ArmorTypeEquipMultiplier;
	TMap<EMMOArmorSlotType, FEquipGeneratorData> ArmorSlotEquipMultiplier;
	TMap<EMMOWeaponType, FEquipGeneratorData> WeaponTypeEquipMultiplier;
	TArray<FMMOEquipGeneratorQualifyingNameultiplierDataTable> QualifyingNameEquipMultiplier;

	void ParseEquipGenerator();
	void ParseArmorTypeGenerator();
	void ParseArmorSlotGenerator();
	void ParseWeaponTypeGenerator();
	void ParseQualifyingNameGenerator();

	void Init();
	void Uninit();

	void ParseAnimationDataTable();
	void ParseRaceCharacterDataTable();
	void ParseCharacterProgressionDataTable();
};
