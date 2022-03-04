// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/MMOCharacterGenerator.h"
#include "MMOGameInstance.h"
#include <random>

UMMOCharacterGenerator* UMMOCharacterGenerator::Instance;

int32 FMMOGaussianGenerator::GetRandomValue() const
{
	static std::default_random_engine generator;
	std::normal_distribution<float> distribution(Mu, StdDev);

	const float Value = distribution(generator);
	return FMath::RoundToInt32(Value);
}

void UMMOCharacterGenerator::Startup(UObject* Outer)
{
	check(!Instance);
	Instance = NewObject<UMMOCharacterGenerator>(Outer);
	Instance->AddToRoot();
	Instance->Init();
}

void UMMOCharacterGenerator::Shutdown()
{
	check(Instance);
	Instance->Uninit();
	Instance->RemoveFromRoot();
	Instance = nullptr;
}

FMMOCharacter UMMOCharacterGenerator::GenerateCharacter(EMMOCharacterClass InClass) const
{
	const FCharacterAttributesGenerator& Generator = AttributesGeneratorMap[InClass];

	FMMOCharacter Character;
	Character.Name = Names[FMath::RandRange(0, Names.Num() - 1)]; // TODO, avoid duplicate names.
	Character.CharacterClass = InClass;
	Character.Gold = FMath::Max(0, Generator.GoldGenerator.GetRandomValue());
	Character.Greediness = FMath::Max(0, Generator.GredinessGenerator.GetRandomValue());
	Character.Happiness = FMath::Max(0, Generator.HappinessGenerator.GetRandomValue());
	Character.Level = 1;
	Character.Attributes =  Generator.GenerateAttributes();
	return Character;
}

void UMMOCharacterGenerator::Init()
{
	UMMOGameInstance* GameInstance = Cast<UMMOGameInstance>(GetOuter());
	UDataTable* CharacterDataTable = GameInstance->RetrieveDataTable(CharacterDataTableName);

	if (!CharacterDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("Character Generator: cannot find CharacterDataTableName"));
		return;
	}

	TArray<FMMOCharacterGeneratorDataTable*> AllRows;
	CharacterDataTable->GetAllRows<FMMOCharacterGeneratorDataTable>(TEXT("CHARACTER_GENERATOR"), AllRows);

	for (FMMOCharacterGeneratorDataTable* Row : AllRows)
	{
		const EMMOCharacterClass WeaponType = Row->Class;
		AttributesGeneratorMap.Add(WeaponType, Row->AttributesGenerator);
	}
}

void UMMOCharacterGenerator::Uninit()
{
	AttributesGeneratorMap.Empty();
}

FMMOCharacterAttributes FCharacterAttributesGenerator::GenerateAttributes() const
{
	FMMOCharacterAttributes Attributes;
	Attributes.Strength = StrengthGenerator.GetRandomValue();
	Attributes.Constitution = ConstitutionGenerator.GetRandomValue();
	Attributes.Dexterity = DexterityGenerator.GetRandomValue();
	Attributes.Intellect = IntellectGenerator.GetRandomValue();

	return Attributes;
}
