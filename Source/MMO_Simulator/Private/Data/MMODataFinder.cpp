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

void UMMODataFinder::ParseCharacterProgressionDataTable()
{
	UMMOGameInstance* GameInstance = Cast<UMMOGameInstance>(GetOuter());
	UDataTable* CharacterProgressionDataTable = GameInstance->RetrieveDataTable(CharacterProgressionTableName);

	if (!CharacterProgressionDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("DataFinder: cannot find CharacterProgressionDataTable"));
		return;
	}

	TArray<FMMOCharacterProgressionDataTable*> AllRows;
	CharacterProgressionDataTable->GetAllRows<FMMOCharacterProgressionDataTable>(TEXT("DATA_RETRIEVER"), AllRows);

	for (FMMOCharacterProgressionDataTable* Row : AllRows)
	{
		AttributesIncreasePerLevel.FindOrAdd(Row->Class) = Row->AttributesIncreasePerLevel;
	}

#if WITH_EDITOR
	UEnum* Enum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EMMOCharacterClass"));
	if (Enum)
	{
		bool bFoundError = false;
		int32 ExpectedValue = -1;

		const int32 EnumNum = Enum->NumEnums();
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
	}
#endif
}
