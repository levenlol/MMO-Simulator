// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/MMOEquipGenerator.h"
#include "MMOGameInstance.h"

UMMOEquipGenerator* UMMOEquipGenerator::Instance = nullptr;

void UMMOEquipGenerator::Startup(UObject* Outer)
{
	check(!Instance);
	Instance = NewObject<UMMOEquipGenerator>(Outer);
	Instance->AddToRoot();
	Instance->Init();
}

void UMMOEquipGenerator::Shutdown()
{
	check(Instance);
	Instance->Uninit();
	Instance->RemoveFromRoot();
	Instance = nullptr;
}

void UMMOEquipGenerator::AddEquipGeneratorData(EMMORarityType Rarity, const FName& DataTableID)
{
	UMMOGameInstance* GameInstance = Cast<UMMOGameInstance>(GetOuter());
	UDataTable* EquipGenDataTable = GameInstance->RetrieveDataTable(DataTableID);

	if (!EquipGenDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("Equip Generator: cannot find %s"), *DataTableID.ToString());
		return;
	}

	TArray<FMMOEquipGeneratorDataTable*> EquipGenRows;
	EquipGenDataTable->GetAllRows<FMMOEquipGeneratorDataTable>(TEXT("EQUIP_GENERATOR"), EquipGenRows);

	for (FMMOEquipGeneratorDataTable* Row : EquipGenRows)
	{
		EquipGeneratorItemLevel.FindOrAdd(Rarity).Data.Add(*Row);
	}
}

void UMMOEquipGenerator::ParseEquipGenerator()
{
	UEnum* RarityEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EMMORarityType"));
	if (!RarityEnum)
	{
		UE_LOG(LogTemp, Fatal, TEXT("Cannot find EMMORarityType"));
		return;
	}

	const int32 RarityEnumNum = RarityEnum->NumEnums();

	for (int32 i = 0; i < RarityEnumNum - 1; i++)
	{
		AddEquipGeneratorData(static_cast<EMMORarityType>(i), *(RarityEnum->GetNameStringByIndex(i) + EquipGenDataTableName));
	}
}

void UMMOEquipGenerator::ParseArmorTypeGenerator()
{
	UMMOGameInstance* GameInstance = Cast<UMMOGameInstance>(GetOuter());
	UDataTable* ArmorTypeGenDataTable = GameInstance->RetrieveDataTable(ArmorTypeDataTableName);

	if (!ArmorTypeGenDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("Equip Generator: cannot find %s"), *ArmorTypeDataTableName.ToString());
		return;
	}

	TArray<FMMOEquipGeneratorArmorTypeMultiplierDataTable*> ArmorRows;
	ArmorTypeGenDataTable->GetAllRows<FMMOEquipGeneratorArmorTypeMultiplierDataTable>(TEXT("EQUIP_GENERATOR"), ArmorRows);

	for (FMMOEquipGeneratorArmorTypeMultiplierDataTable* Row : ArmorRows)
	{
		ArmorTypeEquipMultiplier.FindOrAdd(Row->ArmorType).Data.Add(*Row);
	}
}

void UMMOEquipGenerator::ParseArmorSlotGenerator()
{
	UMMOGameInstance* GameInstance = Cast<UMMOGameInstance>(GetOuter());
	UDataTable* ArmorSlotGenDataTable = GameInstance->RetrieveDataTable(ArmorSlotDataTableName);

	if (!ArmorSlotGenDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("Equip Generator: cannot find %s"), *ArmorSlotDataTableName.ToString());
		return;
	}

	TArray<FMMOEquipGeneratorArmorSlotMultiplierDataTable*> ArmorRows;
	ArmorSlotGenDataTable->GetAllRows<FMMOEquipGeneratorArmorSlotMultiplierDataTable>(TEXT("EQUIP_GENERATOR"), ArmorRows);

	for (FMMOEquipGeneratorArmorSlotMultiplierDataTable* Row : ArmorRows)
	{
		ArmorSlotEquipMultiplier.FindOrAdd(Row->ArmorSlot).Data.Add(*Row);
	}
}

void UMMOEquipGenerator::ParseWeaponTypeGenerator()
{
	UMMOGameInstance* GameInstance = Cast<UMMOGameInstance>(GetOuter());
	UDataTable* WeaponTypeGenDataTable = GameInstance->RetrieveDataTable(WeaponTypeDataTableName);

	if (!WeaponTypeGenDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("Equip Generator: cannot find %s"), *WeaponTypeDataTableName.ToString());
		return;
	}

	TArray<FMMOEquipGeneratorWeaponTypeMultiplierDataTable*> WeaponRows;
	WeaponTypeGenDataTable->GetAllRows<FMMOEquipGeneratorWeaponTypeMultiplierDataTable>(TEXT("EQUIP_GENERATOR"), WeaponRows);

	for (FMMOEquipGeneratorWeaponTypeMultiplierDataTable* Row : WeaponRows)
	{
		WeaponTypeEquipMultiplier.FindOrAdd(Row->WeaponType).Data.Add(*Row);
	}
}

void UMMOEquipGenerator::Init()
{
	ParseEquipGenerator();
	ParseArmorTypeGenerator();
	ParseArmorSlotGenerator();
	ParseWeaponTypeGenerator();
}

void UMMOEquipGenerator::Uninit()
{
	EquipGeneratorItemLevel.Empty();
}

const FMMOEquipGeneratorDataTable& FEquipGeneratorData::GetEquipGeneratorForItemLevel(int32 ItemLevel) const
{
	ItemLevel = FMath::Max(0, ItemLevel - 1); // convert to 0 based index

	if (Data.IsValidIndex(ItemLevel))
	{
		return Data[ItemLevel];
	}

	static FMMOEquipGeneratorDataTable ZeroData;
	return ZeroData;
}
