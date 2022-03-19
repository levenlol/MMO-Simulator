// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "MMOEquipGenerator.generated.h"

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
struct MMO_SIMULATOR_API FEquipGeneratorData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	TArray<FMMOEquipGeneratorDataTable> Data;

	const FMMOEquipGeneratorDataTable& GetEquipGeneratorForItemLevel(int32 ItemLevel) const;
};

UCLASS(config = game, defaultconfig)
class MMO_SIMULATOR_API UMMOEquipGenerator : public UObject
{
	GENERATED_BODY()
public:
	static void Startup(UObject* Outer);
	static void Shutdown();

	UPROPERTY(config)
	FString EquipGenDataTableName;

	UPROPERTY(config)
	FName ArmorTypeDataTableName;

	UPROPERTY(config)
	FName ArmorSlotDataTableName;

	UPROPERTY(config)
	FName WeaponTypeDataTableName;

private:
	static UMMOEquipGenerator* Instance;

	void AddEquipGeneratorData(EMMORarityType Rarity, const FName& DataTableID);

	TMap<EMMORarityType, FEquipGeneratorData> EquipGeneratorItemLevel;
	TMap<EMMOArmorType, FEquipGeneratorData> ArmorTypeEquipMultiplier;
	TMap<EMMOArmorSlotType, FEquipGeneratorData> ArmorSlotEquipMultiplier;
	TMap<EMMOWeaponType, FEquipGeneratorData> WeaponTypeEquipMultiplier;

	void ParseEquipGenerator();
	void ParseArmorTypeGenerator();
	void ParseArmorSlotGenerator();
	void ParseWeaponTypeGenerator();

	void Init();
	void Uninit();
};
