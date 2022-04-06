// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/MMOEquipGenerator.h"
#include "MMOGameInstance.h"
#include "Data/MMODataFinder.h"

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

EMMORarityType UMMOEquipGenerator::PickRarityFromProbabilityDistribution(TArray<float> ProbabilityDistribution) const
{
	const int32 RarityNum = RarityEnum->NumEnums() - 1;
	const int32 SafeMaxNum = FMath::Min(RarityNum, ProbabilityDistribution.Num());

	if (ProbabilityDistribution.Num() == 0)
	{
		UE_LOG(LogTemp, Fatal, TEXT("Cannot pick a random Rarity from a NULL probability distribution"));
	}

	// Normalize probability
	float TotalProb = 0.f;
	for (int32 i = 0; i < SafeMaxNum; i++)
	{
		const float P = ProbabilityDistribution[i];
		TotalProb += P;
	}

	for (int32 i = 0; i < SafeMaxNum; i++)
	{
		float& P = ProbabilityDistribution[i];
		P /= TotalProb;
	}

	const float Rand = FMath::FRand();
	float Acc = 0.f;
	for (int32 i = 0; i < SafeMaxNum; i++)
	{
		const float P = ProbabilityDistribution[i];
		Acc += P;
		if (Acc >= Rand)
		{
			return static_cast<EMMORarityType>(i);
		}
	}

	return static_cast<EMMORarityType>(RarityNum - 1);
}

FMMOItemStats UMMOEquipGenerator::GenerateEquipOfKind(int32 ItemLevel, EMMORarityType Rarity, EMMOArmorSlotType ArmorSlotType, EMMOArmorType ArmorType, const FString& Qualifier)
{
	UMMODataFinder* DataFinder = UMMODataFinder::Get();
	ItemLevel = FMath::Max(ItemLevel, 1);

	// retrieve data for generation
	const FMMOEquipGeneratorDataTable& EquipGenerator = DataFinder->GetEquipGeneratorData(Rarity, ItemLevel);
	const FMMOEquipGeneratorDataTable& EquipArmorTypeGenerator = DataFinder->GetEquipGeneratorArmorTypeMultipliers(ArmorType);
	const FMMOEquipGeneratorDataTable& EquipArmorSlotGenerator = DataFinder->GetEquipGeneratorArmorSlotMultipliers(ArmorSlotType);
	const FMMOEquipGeneratorQualifyingNameultiplierDataTable& EquipQualifyGenerator = Qualifier.IsEmpty() ? DataFinder->GetEquipGeneratorRandomQualifyingMultipliers() 
		: DataFinder->GetEquipGeneratorQualifyingMultipliers(Qualifier);

	FMMOItemStats EquipStats;
	EquipStats.ItemName = GetEquipSlotName(ArmorSlotType) + TEXT(" ") + EquipQualifyGenerator.QualifyingName;
	EquipStats.ItemLevel = ItemLevel;
	EquipStats.Rarity = Rarity;
	EquipStats.Durability = 100;
	EquipStats.ArmorSlot = ArmorSlotType;
	EquipStats.ArmorType = ArmorType;

	// Base stats
	EquipStats.Attributes.Strength = EquipGenerator.Strength;
	EquipStats.Attributes.Intellect = EquipGenerator.Intellect;
	EquipStats.Attributes.Constitution = EquipGenerator.Constitution;
	EquipStats.Attributes.Dexterity = EquipGenerator.Dexterity;
	EquipStats.CombatAttributes.AttackCritChanceRating = EquipGenerator.MeleeCritRating;
	EquipStats.CombatAttributes.AttackPower = EquipGenerator.AttackPower;
	EquipStats.CombatAttributes.BlockRating = EquipGenerator.BlockRating;
	EquipStats.CombatAttributes.DodgeRating = EquipGenerator.DodgeRating;
	EquipStats.CombatAttributes.SpellPower = EquipGenerator.SpellPower;
	EquipStats.CombatAttributes.SpellCritChanceRating = EquipGenerator.SpellCritRating;
	EquipStats.Resistances.Armor = EquipGenerator.Armor;
	EquipStats.Resistances.ArcaneResistance = EquipGenerator.ArcaneResistance;
	EquipStats.Resistances.FireResistance = EquipGenerator.FireResistance;
	EquipStats.Resistances.HolyResistance = EquipGenerator.HolyResistance;
	EquipStats.Resistances.IceResistance = EquipGenerator.IceResistance;
	EquipStats.Resistances.NatureResistance = EquipGenerator.NatureResistance;
	EquipStats.Resistances.ShadowResistance = EquipGenerator.ShadowResistance;
	

	// Apply multipliers
	// Armor type multipliers
	if (ArmorSlotType != EMMOArmorSlotType::Ring && ArmorSlotType != EMMOArmorSlotType::Trinket)
	{
		EquipStats.Attributes.Strength *= EquipArmorTypeGenerator.Strength / 100.f;
		EquipStats.Attributes.Intellect *= EquipArmorTypeGenerator.Intellect / 100.f;
		EquipStats.Attributes.Constitution *= EquipArmorTypeGenerator.Constitution / 100.f;
		EquipStats.Attributes.Dexterity *= EquipArmorTypeGenerator.Dexterity / 100.f;
		EquipStats.CombatAttributes.AttackCritChanceRating *= EquipArmorTypeGenerator.MeleeCritRating / 100.f;
		EquipStats.CombatAttributes.AttackPower *= EquipArmorTypeGenerator.AttackPower / 100.f;
		EquipStats.CombatAttributes.BlockRating *= EquipArmorTypeGenerator.BlockRating / 100.f;
		EquipStats.CombatAttributes.DodgeRating *= EquipArmorTypeGenerator.DodgeRating / 100.f;
		EquipStats.CombatAttributes.SpellPower *= EquipArmorTypeGenerator.SpellPower / 100.f;
		EquipStats.CombatAttributes.SpellCritChanceRating *= EquipArmorTypeGenerator.SpellCritRating / 100.f;
		EquipStats.Resistances.Armor *= EquipArmorTypeGenerator.Armor / 100.f;
		EquipStats.Resistances.ArcaneResistance *= EquipArmorTypeGenerator.ArcaneResistance / 100.f;
		EquipStats.Resistances.FireResistance *= EquipArmorTypeGenerator.FireResistance / 100.f;
		EquipStats.Resistances.HolyResistance *= EquipArmorTypeGenerator.HolyResistance / 100.f;
		EquipStats.Resistances.IceResistance *= EquipArmorTypeGenerator.IceResistance / 100.f;
		EquipStats.Resistances.NatureResistance *= EquipArmorTypeGenerator.NatureResistance / 100.f;
		EquipStats.Resistances.ShadowResistance *= EquipArmorTypeGenerator.ShadowResistance / 100.f;
	}
	
	// Armor slot multipliers
	EquipStats.Attributes.Strength *= EquipArmorSlotGenerator.Strength / 100.f;
	EquipStats.Attributes.Intellect *= EquipArmorSlotGenerator.Intellect / 100.f;
	EquipStats.Attributes.Constitution *= EquipArmorSlotGenerator.Constitution / 100.f;
	EquipStats.Attributes.Dexterity *= EquipArmorSlotGenerator.Dexterity / 100.f;
	EquipStats.CombatAttributes.AttackCritChanceRating *= EquipArmorSlotGenerator.MeleeCritRating / 100.f;
	EquipStats.CombatAttributes.AttackPower *= EquipArmorSlotGenerator.AttackPower / 100.f;
	EquipStats.CombatAttributes.BlockRating *= EquipArmorSlotGenerator.BlockRating / 100.f;
	EquipStats.CombatAttributes.DodgeRating *= EquipArmorSlotGenerator.DodgeRating / 100.f;
	EquipStats.CombatAttributes.SpellPower *= EquipArmorSlotGenerator.SpellPower / 100.f;
	EquipStats.CombatAttributes.SpellCritChanceRating *= EquipArmorSlotGenerator.SpellCritRating / 100.f;
	EquipStats.Resistances.Armor *= EquipArmorSlotGenerator.Armor / 100.f;
	EquipStats.Resistances.ArcaneResistance *= EquipArmorSlotGenerator.ArcaneResistance / 100.f;
	EquipStats.Resistances.FireResistance *= EquipArmorSlotGenerator.FireResistance / 100.f;
	EquipStats.Resistances.HolyResistance *= EquipArmorSlotGenerator.HolyResistance / 100.f;
	EquipStats.Resistances.IceResistance *= EquipArmorSlotGenerator.IceResistance / 100.f;
	EquipStats.Resistances.NatureResistance *= EquipArmorSlotGenerator.NatureResistance / 100.f;
	EquipStats.Resistances.ShadowResistance *= EquipArmorSlotGenerator.ShadowResistance / 100.f;

	// Qualifier
	EquipStats.Attributes.Strength *= EquipQualifyGenerator.Strength / 100.f;
	EquipStats.Attributes.Intellect *= EquipQualifyGenerator.Intellect / 100.f;
	EquipStats.Attributes.Constitution *= EquipQualifyGenerator.Constitution / 100.f;
	EquipStats.Attributes.Dexterity *= EquipQualifyGenerator.Dexterity / 100.f;
	EquipStats.CombatAttributes.AttackCritChanceRating *= EquipQualifyGenerator.MeleeCritRating / 100.f;
	EquipStats.CombatAttributes.AttackPower *= EquipQualifyGenerator.AttackPower / 100.f;
	EquipStats.CombatAttributes.BlockRating *= EquipQualifyGenerator.BlockRating / 100.f;
	EquipStats.CombatAttributes.DodgeRating *= EquipQualifyGenerator.DodgeRating / 100.f;
	EquipStats.CombatAttributes.SpellPower *= EquipQualifyGenerator.SpellPower / 100.f;
	EquipStats.CombatAttributes.SpellCritChanceRating *= EquipQualifyGenerator.SpellCritRating / 100.f;
	EquipStats.Resistances.Armor *= EquipQualifyGenerator.Armor / 100.f;
	EquipStats.Resistances.ArcaneResistance *= EquipQualifyGenerator.ArcaneResistance / 100.f;
	EquipStats.Resistances.FireResistance *= EquipQualifyGenerator.FireResistance / 100.f;
	EquipStats.Resistances.HolyResistance *= EquipQualifyGenerator.HolyResistance / 100.f;
	EquipStats.Resistances.IceResistance *= EquipQualifyGenerator.IceResistance / 100.f;
	EquipStats.Resistances.NatureResistance *= EquipQualifyGenerator.NatureResistance / 100.f;
	EquipStats.Resistances.ShadowResistance *= EquipQualifyGenerator.ShadowResistance / 100.f;
	
	return EquipStats;
}

FString UMMOEquipGenerator::GetEquipSlotName(EMMOArmorSlotType ArmorSlotType) const
{
	return ArmorSlotEnum->GetNameStringByValue(static_cast<int64>(ArmorSlotType));
}

void UMMOEquipGenerator::Init()
{
	ArmorSlotEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EMMOArmorSlotType"));
	if (!ArmorSlotEnum)
	{
		UE_LOG(LogTemp, Fatal, TEXT("Cannot find enum EMMOArmorSlotType"));
	}

	RarityEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EMMORarityType"));
	if (!RarityEnum)
	{
		UE_LOG(LogTemp, Fatal, TEXT("Cannot find enum EMMORarityType"));
	}
}

void UMMOEquipGenerator::Uninit()
{
	ArmorSlotEnum = nullptr;
	RarityEnum = nullptr;
}

