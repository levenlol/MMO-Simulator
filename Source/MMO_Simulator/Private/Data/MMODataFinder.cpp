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

FMMOCharacterAttributes UMMODataFinder::GetCharacterProgressionForLevels(const EMMOCharacterClass CharacterClass, int32 StartLevel, int32 EndLevel) const
{
	FMMOCharacterAttributes Progression;

	if (AttributesIncreasePerLevel.Contains(CharacterClass))
	{
		const TArray<FMMOCharacterAttributes>& AttributesPerLevel = AttributesIncreasePerLevel[CharacterClass].AttributesIncreasePerLevel;
		const int32 StartIndex = FMath::Max(StartLevel - 1, 0);
		const int32 EndIndex = FMath::Max(EndLevel - 1, 0);

		for (int32 i = StartIndex; i <= EndIndex; i++)
		{
			if (AttributesPerLevel.IsValidIndex(i))
			{
				Progression.Strength += AttributesPerLevel[i].Strength;
				Progression.Dexterity += AttributesPerLevel[i].Dexterity;
				Progression.Intellect += AttributesPerLevel[i].Intellect;
				Progression.Constitution += AttributesPerLevel[i].Constitution;
			}
		}
	}

	return Progression;
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

void UMMODataFinder::Init()
{
	ParseAnimationDataTable();
	ParseRaceCharacterDataTable();
	ParseCharacterProgressionDataTable();
}

void UMMODataFinder::Uninit()
{
	AnimationsMap.Empty();
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
