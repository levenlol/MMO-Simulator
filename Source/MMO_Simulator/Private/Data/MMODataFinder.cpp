// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/MMODataFinder.h"
#include "Misc/MessageDialog.h"
#include "Engine/DataTable.h"
#include "MMOGameInstance.h"

UMMODataFinder* UMMODataFinder::Instance{ nullptr };

void UMMODataFinder::Startup(UObject* Outer)
{
	check(!Instance);
	Instance = NewObject<UMMODataFinder>(Outer);
	Instance->AddToRoot();
	Instance->Init();
}

void UMMODataFinder::Shutdown()
{
	check(Instance);
	Instance->Uninit();
	Instance->RemoveFromRoot();
	Instance = nullptr;
}

FMMOCharacterAttributes UMMODataFinder::GetCharacterProgressionAtLevel(const EMMOCharacterClass CharacterClass, int32 Level) const
{
	if (AttributesIncreasePerLevel.Contains(CharacterClass))
	{
		const TArray<FMMOCharacterAttributes>& AttributesPerLevel = AttributesIncreasePerLevel[CharacterClass].AttributesIncreasePerLevel;

		if (AttributesPerLevel.IsValidIndex(Level))
		{
			return AttributesPerLevel[Level];
		}
	}
	
	UE_LOG(LogTemp, Error, TEXT("Didnt find a CharacterProgression for level: %d"), Level);
	return FMMOCharacterAttributes();
}

FMMOCharacterAttributes UMMODataFinder::GetRaceAttributes(const EMMOCharacterRace InRace) const
{
	if (RaceAttributes.Contains(InRace))
	{
		return RaceAttributes[InRace];
	}

	return FMMOCharacterAttributes();
}

UAnimSequenceBase* UMMODataFinder::GetAnimSequence(const FMMOWeaponTypeCouple& WeaponCouple) const
{
	if (AnimationsMap.Contains(WeaponCouple))
	{
		return AnimationsMap[WeaponCouple];
	}

	return nullptr;
}

const FMMOEquipGeneratorDataTable& UMMODataFinder::GetEquipGeneratorData(EMMORarityType RarityType, int32 ItemLevel) const
{
	if (EquipGeneratorItemLevel.Contains(RarityType))
	{
		const int32 ItemLevelIdx = FMath::Max(0, ItemLevel - 1); // 0 based index

		if (EquipGeneratorItemLevel[RarityType].Data.IsValidIndex(ItemLevelIdx))
		{
			return EquipGeneratorItemLevel[RarityType].Data[ItemLevelIdx];
		}
	}

	UE_LOG(LogTemp, Error, TEXT("Cannot find item generation data for Rarity: %d, ItemLevel: %d"), static_cast<int32>(RarityType), ItemLevel);

	static FMMOEquipGeneratorDataTable ZeroData;
	return ZeroData;
}

const FMMOEquipGeneratorDataTable& UMMODataFinder::GetEquipGeneratorArmorTypeMultipliers(EMMOArmorType ArmorType) const
{
	if (ArmorTypeEquipMultiplier.Contains(ArmorType))
	{
		if (ArmorTypeEquipMultiplier[ArmorType].Data.IsValidIndex(0))
		{
			return ArmorTypeEquipMultiplier[ArmorType].Data[0];
		}
	}

	UE_LOG(LogTemp, Error, TEXT("Cannot find item generation data for ArmorType: %d"), static_cast<int32>(ArmorType));

	static FMMOEquipGeneratorDataTable ZeroData;
	return ZeroData;
}

const FMMOEquipGeneratorDataTable& UMMODataFinder::GetEquipGeneratorArmorSlotMultipliers(EMMOArmorSlotType ArmorSlot) const
{
	if (ArmorSlotEquipMultiplier.Contains(ArmorSlot))
	{
		if (ArmorSlotEquipMultiplier[ArmorSlot].Data.IsValidIndex(0))
		{
			return ArmorSlotEquipMultiplier[ArmorSlot].Data[0];
		}
	}

	UE_LOG(LogTemp, Error, TEXT("Cannot find item generation data for ArmorSlot: %d"), static_cast<int32>(ArmorSlot));

	static FMMOEquipGeneratorDataTable ZeroData;
	return ZeroData;
}

const FMMOEquipGeneratorDataTable& UMMODataFinder::GetEquipGeneratorWeaponTypeMultipliers(EMMOWeaponType WeaponType) const
{
	if (WeaponTypeEquipMultiplier.Contains(WeaponType))
	{
		if (WeaponTypeEquipMultiplier[WeaponType].Data.IsValidIndex(0))
		{
			return WeaponTypeEquipMultiplier[WeaponType].Data[0];
		}
	}

	UE_LOG(LogTemp, Error, TEXT("Cannot find item generation data for ArmorSlot: %d"), static_cast<int32>(WeaponType));

	static FMMOEquipGeneratorDataTable ZeroData;
	return ZeroData;
}

const FMMOEquipGeneratorQualifyingNameultiplierDataTable& UMMODataFinder::GetEquipGeneratorRandomQualifyingMultipliers() const
{
	if (QualifyingNameEquipMultiplier.Num() > 0)
	{
		return QualifyingNameEquipMultiplier[FMath::RandRange(0, QualifyingNameEquipMultiplier.Num() - 1)];
	}

	UE_LOG(LogTemp, Error, TEXT("GetEquipGeneratorRandomQualifyingMultipliers didnt have data"));

	static FMMOEquipGeneratorQualifyingNameultiplierDataTable ZeroData;
	return ZeroData;
}

const FMMOEquipGeneratorQualifyingNameultiplierDataTable& UMMODataFinder::GetEquipGeneratorQualifyingMultipliers(const FString& Qualifier) const
{
	const FMMOEquipGeneratorQualifyingNameultiplierDataTable* Element = QualifyingNameEquipMultiplier.FindByPredicate([Qualifier](const FMMOEquipGeneratorQualifyingNameultiplierDataTable& Element)
		{
			return Element.QualifyingName.Equals(Qualifier, ESearchCase::Type::IgnoreCase);
		});

	if (Element)
	{
		return *Element;
	}

	UE_LOG(LogTemp, Error, TEXT("GetEquipGeneratorRandomQualifyingMultipliers didnt find: %s qualifier"), *Qualifier);

	static FMMOEquipGeneratorQualifyingNameultiplierDataTable ZeroData;
	return ZeroData;
}

void UMMODataFinder::AddEquipGeneratorData(EMMORarityType Rarity, const FName& DataTableID)
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

void UMMODataFinder::ParseEquipGenerator()
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

void UMMODataFinder::ParseArmorTypeGenerator()
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

void UMMODataFinder::ParseArmorSlotGenerator()
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

void UMMODataFinder::ParseWeaponTypeGenerator()
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

void UMMODataFinder::ParseQualifyingNameGenerator()
{
	UMMOGameInstance* GameInstance = Cast<UMMOGameInstance>(GetOuter());
	UDataTable* QualifyingGenDataTable = GameInstance->RetrieveDataTable(QualifyingNameDataTableName);

	if (!QualifyingGenDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("Equip Generator: cannot find %s"), *QualifyingNameDataTableName.ToString());
		return;
	}

	TArray<FMMOEquipGeneratorQualifyingNameultiplierDataTable*> Rows;
	QualifyingGenDataTable->GetAllRows<FMMOEquipGeneratorQualifyingNameultiplierDataTable>(TEXT("EQUIP_GENERATOR"), Rows);

	Algo::Transform(Rows, QualifyingNameEquipMultiplier, [](const FMMOEquipGeneratorQualifyingNameultiplierDataTable* InData)
		{
			return *InData;
		});
}


void UMMODataFinder::Init()
{
	ParseAnimationDataTable();
	ParseRaceCharacterDataTable();
	ParseCharacterProgressionDataTable();

	ParseEquipGenerator();
	ParseArmorTypeGenerator();
	ParseArmorSlotGenerator();
	ParseWeaponTypeGenerator();
	ParseQualifyingNameGenerator();
}

void UMMODataFinder::Uninit()
{
	AnimationsMap.Empty();
	EquipGeneratorItemLevel.Empty();
}

void UMMODataFinder::ParseAnimationDataTable()
{
	UMMOGameInstance* GameInstance = Cast<UMMOGameInstance>(GetOuter());
	UDataTable* AnimationDataTable = GameInstance->RetrieveDataTable(AnimationDataTableName);

	if (!AnimationDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("DataFinder: cannot find AnimationDatatable"));
		return;
	}

	TArray<FMMOAnimationDataTable*> AllRows;
	AnimationDataTable->GetAllRows<FMMOAnimationDataTable>(TEXT("DATA_RETRIEVER"), AllRows);

	for (FMMOAnimationDataTable* Row : AllRows)
	{
		const FMMOWeaponTypeCouple& WeaponType = Row->WeaponsType;
#if WITH_EDITORONLY_DATA
		if (AnimationsMap.Contains(WeaponType) || !Row->WeaponAnimation)
		{
			const FText Title = FText::FromString(TEXT("Ciavola Fixa"));
			FMessageDialog::Open(EAppMsgType::Type::Ok, FText::FromString(TEXT("Found Duplicated Row or Null animation")), &Title);
		}
		else
#endif
		{
			AnimationsMap.Add(WeaponType, Row->WeaponAnimation);
		}
	}
}

void UMMODataFinder::ParseRaceCharacterDataTable()
{
	UMMOGameInstance* GameInstance = Cast<UMMOGameInstance>(GetOuter());
	UDataTable* CharacterRaceDataTable = GameInstance->RetrieveDataTable(RaceDataTableName);

	if (!CharacterRaceDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("DataFinder: cannot find CharacterRaceDataTable"));
		return;
	}

	TArray<FMMORaceProgressionDataTable*> AllRows;
	CharacterRaceDataTable->GetAllRows<FMMORaceProgressionDataTable>(TEXT("DATA_RETRIEVER"), AllRows);

	for (FMMORaceProgressionDataTable* Row : AllRows)
	{
		FMMOCharacterAttributes Attributes;
		Attributes.Strength = Row->Strength;
		Attributes.Intellect = Row->Intellect;
		Attributes.Dexterity = Row->Dexterity;
		Attributes.Constitution = Row->Constitution;

		RaceAttributes.FindOrAdd(Row->Race) = Attributes;
	}

#if WITH_EDITOR
	UEnum* Enum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EMMOCharacterRace"));
	if (!Enum)
	{
		UE_LOG(LogTemp, Fatal, TEXT("UMMODataFinder Cannot Find EMMOCharacterRace."));
		return;
	}

	const int32 EnumNum = Enum->NumEnums();

	FString ErrorString;

	for (int32 i = 0; i < EnumNum - 1; i++)
	{
		EMMOCharacterRace CurrenRace = static_cast<EMMOCharacterRace>(i);

		if (!RaceAttributes.Contains(CurrenRace))
		{
			ErrorString += Enum->GetNameStringByIndex(i) + TEXT(", ");
		}
	}

	if (!ErrorString.IsEmpty())
	{
		const FText Title = FText::FromString(TEXT("Ciavola Fixa"));
		FMessageDialog::Open(EAppMsgType::Type::Ok, FText::FromString(TEXT("Found error: Missing CharacterRaceProgression for races: ") + ErrorString), &Title);
	}

#endif
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


void UMMODataFinder::ParseCharacterProgressionDataTable()
{
	UMMOGameInstance* GameInstance = Cast<UMMOGameInstance>(GetOuter());

	UEnum* Enum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EMMOCharacterClass"));
	if (!Enum)
	{
		UE_LOG(LogTemp, Fatal, TEXT("UMMODataFinder Cannot Find EMMOCharacterClass."));
		return;
	}

	const int32 EnumNum = Enum->NumEnums();
	for (int32 i = 0; i < EnumNum - 1; i++)
	{
		EMMOCharacterClass CurrenClass = static_cast<EMMOCharacterClass>(i);
		const FString CharacterEnumStr = Enum->GetNameStringByIndex(i);
		const FName DataTableName = *(CharacterEnumStr + CharacterProgressionTableName);

		UDataTable* CharacterProgressionDataTable = GameInstance->RetrieveDataTable(DataTableName);
		if (!CharacterProgressionDataTable)
		{
			UE_LOG(LogTemp, Error, TEXT("DataFinder: cannot find %sDataTable"), *DataTableName.ToString());
			continue;
		}

		TArray<FMMOCharacterProgressionDataTable*> AllRows;
		CharacterProgressionDataTable->GetAllRows<FMMOCharacterProgressionDataTable>(TEXT("DATA_RETRIEVER"), AllRows);

		auto& IncreasePerLevel = AttributesIncreasePerLevel.FindOrAdd(CurrenClass);
		for (FMMOCharacterProgressionDataTable* Row : AllRows)
		{
			FMMOCharacterAttributes Attribute;
			Attribute.Strength = Row->Strength;
			Attribute.Intellect = Row->Intellect;
			Attribute.Dexterity = Row->Dexterity;
			Attribute.Constitution = Row->Constitution;

			IncreasePerLevel.AttributesIncreasePerLevel.Add(Attribute);
		}
	}


#if WITH_EDITOR
	
	bool bFoundError = false;
	int32 ExpectedValue = -1;

	for (int32 i = 0; i < EnumNum - 1; i++)
	{
		EMMOCharacterClass CurrenClass = static_cast<EMMOCharacterClass>(i);
		if (!AttributesIncreasePerLevel.Contains(CurrenClass))
		{
			const FText Title = FText::FromString(TEXT("Ciavola Fixa"));
			FMessageDialog::Open(EAppMsgType::Type::Ok, FText::FromString(TEXT("Missing entries for: ") + Enum->GetNameStringByIndex(i)), &Title);
			continue;
		}

		if (ExpectedValue < 0) ExpectedValue = AttributesIncreasePerLevel[CurrenClass].AttributesIncreasePerLevel.Num();

		if (ExpectedValue != AttributesIncreasePerLevel[CurrenClass].AttributesIncreasePerLevel.Num())
		{
			bFoundError = true;
			ExpectedValue = AttributesIncreasePerLevel[CurrenClass].AttributesIncreasePerLevel.Num();
		}
	}

	if (bFoundError)
	{
		const FText Title = FText::FromString(TEXT("Ciavola Fixa"));
		FMessageDialog::Open(EAppMsgType::Type::Ok, FText::FromString(TEXT("Found error: Inconsistent CharacterProgression data, different levels Num.")), &Title);
	}
#endif
}
