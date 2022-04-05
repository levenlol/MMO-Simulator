// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/MMOStatsManager.h"
#include "Characters/MMOBaseCharacter.h"
#include "Data/MMODataFinder.h"

UMMOStatsManager::UMMOStatsManager()
	: Super()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Init block
	BlockExpression.VariableMap.Add(FString("Strength"), FString("OwnerCharacter.CharacterInfo.Attributes.Strength"));
	BlockExpression.VariableMap.Add(FString("BlockRating"), FString("OwnerCharacter.CharacterInfo.CombatAttributes.BlockRating"));
	BlockExpression.Expression = FString("2 + Strength / 25 + BlockRating / 25");

	// Init dodge
	DodgeExpression.VariableMap.Add(FString("Dexterity"), FString("OwnerCharacter.CharacterInfo.Attributes.Dexterity"));
	DodgeExpression.VariableMap.Add(FString("DodgeRating"), FString("OwnerCharacter.CharacterInfo.CombatAttributes.DodgeRating"));
	DodgeExpression.Expression = FString("2 + Dexterity / 25 + DodgeRating / 25");

	// Init attack power
	AttackPowerExpression.VariableMap.Add(FString("Dexterity"), FString("OwnerCharacter.CharacterInfo.Attributes.Dexterity"));
	AttackPowerExpression.VariableMap.Add(FString("Strength"), FString("OwnerCharacter.CharacterInfo.Attributes.Strength"));
	AttackPowerExpression.Expression = FString("2 * Dexterity + 2 * Strength");

	// Init spell power
	SpellPowerExpression.VariableMap.Add(FString("Intellect"), FString("OwnerCharacter.CharacterInfo.Attributes.Intellect"));
	SpellPowerExpression.Expression = FString("Intellect * 0.8");

	// Init attack crit 
	AttackCritExpression.VariableMap.Add(FString("Dexterity"), FString("OwnerCharacter.CharacterInfo.Attributes.Dexterity"));
	AttackCritExpression.VariableMap.Add(FString("AttackCritChanceRating"), FString("OwnerCharacter.CharacterInfo.CombatAttributes.AttackCritChanceRating"));
	AttackCritExpression.Expression = FString("0.05 + Dexterity / 25 + AttackCritChanceRating / 25");

	// Init spell crit
	SpellCritExpression.VariableMap.Add(FString("Intellect"), FString("OwnerCharacter.CharacterInfo.Attributes.Intellect"));
	SpellCritExpression.VariableMap.Add(FString("SpellCritChanceRating"), FString("OwnerCharacter.CharacterInfo.CombatAttributes.SpellCritChanceRating"));
	SpellCritExpression.Expression = FString("0.05 + Intellect / 25 + SpellCritChanceRating / 25");

	// Init health
	HealthExpression.VariableMap.Add(FString("Constitution"), FString("OwnerCharacter.CharacterInfo.Attributes.Constitution"));
	HealthExpression.Expression = FString("Constitution * 10");

	// Init mana
	ManaExpression.VariableMap.Add(FString("Intellect"), FString("OwnerCharacter.CharacterInfo.Attributes.Intellect"));
	ManaExpression.Expression = FString("Intellect * 0.8");
}

void UMMOStatsManager::OnCharacterInitialized(AMMOBaseCharacter* Sender)
{
	check(Sender == OwnerCharacter);

	RecomputeAttributes();
}

void UMMOStatsManager::OnArmorEquipped(AMMOBaseCharacter* Sender, EMMOArmorSlotType Slot, const FMMOItemStats& Item)
{
	RecomputeAttributes();
}

void UMMOStatsManager::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<AMMOBaseCharacter>(GetOwner());

	if (OwnerCharacter)
	{
		OwnerCharacter->OnCharacterInitialized.AddUniqueDynamic(this, &UMMOStatsManager::OnCharacterInitialized);
		OwnerCharacter->OnArmorEquipped.AddUniqueDynamic(this, &UMMOStatsManager::OnArmorEquipped);
	}

	BlockExpression.Init(this);
	DodgeExpression.Init(this);
	AttackPowerExpression.Init(this);
	SpellPowerExpression.Init(this);
	AttackCritExpression.Init(this);
	SpellCritExpression.Init(this);
	HealthExpression.Init(this);
	ManaExpression.Init(this);

	RecomputeAttributes();
}

void UMMOStatsManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	BlockExpression.Release();
	DodgeExpression.Release();
	AttackCritExpression.Release();
	SpellCritExpression.Release();
	AttackPowerExpression.Release();
	SpellPowerExpression.Release();
	HealthExpression.Release();
	ManaExpression.Release();

	if (OwnerCharacter)
	{
		OwnerCharacter->OnCharacterInitialized.RemoveDynamic(this, &UMMOStatsManager::OnCharacterInitialized);
		OwnerCharacter->OnArmorEquipped.RemoveDynamic(this, &UMMOStatsManager::OnArmorEquipped);
	}
}

void UMMOStatsManager::RecomputeAttributes()
{
	RecomputeBaseAttributes();
	UpdateCharacterAttributes();
}

void UMMOStatsManager::RecomputeBaseAttributes()
{
	// Compute new base stats
	BaseAttributes = OwnerCharacter->CharacterInfo.InitialAttributes;
	// add attributes gained from level
	BaseAttributes = BaseAttributes + UMMODataFinder::Get()->GetCharacterProgressionAtLevel(OwnerCharacter->CharacterInfo.CharacterClass, OwnerCharacter->CharacterInfo.Level);

	// add attributes for race
	FMMOCharacterAttributes RaceAttributes = UMMODataFinder::Get()->GetRaceAttributes(OwnerCharacter->CharacterInfo.Race);
	BaseAttributes.Strength *= RaceAttributes.Strength / 100.f;
	BaseAttributes.Intellect *= RaceAttributes.Intellect / 100.f;
	BaseAttributes.Constitution *= RaceAttributes.Constitution / 100.f;
	BaseAttributes.Dexterity *= RaceAttributes.Dexterity / 100.f;
}

void UMMOStatsManager::RecomputeCombatAttributes()
{
	FMMOCombatAttributes CombatAttributes;
	for (const TPair<EMMOArmorSlotType, FMMOItemStats>& Item : OwnerCharacter->EquippedArmor)
	{
		CombatAttributes = CombatAttributes + Item.Value.CombatAttributes;
	}

	OwnerCharacter->CharacterInfo.CombatAttributes = CombatAttributes;
}

void UMMOStatsManager::RecomputeHealthAndResources()
{
	if (!OwnerCharacter)
		return;

	if (HealthExpression.IsValid())
	{
		OwnerCharacter->CharacterInfo.Stats.MaxHealth = HealthExpression.Eval<int32>(false);
	}

	if (ManaExpression.IsValid())
	{
		OwnerCharacter->CharacterInfo.Stats.MaxResources = ManaExpression.Eval<int32>(false);
	}
}

void UMMOStatsManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UMMOStatsManager::UpdateCharacterAttributes()
{
	if (OwnerCharacter)
	{
		// Sum equip stats.
		FMMOCharacterAttributes FullAttributes = BaseAttributes;
		for (const TPair<EMMOArmorSlotType, FMMOItemStats>& Item : OwnerCharacter->EquippedArmor)
		{
			FullAttributes = FullAttributes + Item.Value.Attributes;
		}

		OwnerCharacter->CharacterInfo.Attributes = FullAttributes;

		// recompute all other stuff
		ComputeSecondaryAttributes();
	}
}

void UMMOStatsManager::RefreshMathExpressions()
{
	BlockExpression.RefreshVariables();
	DodgeExpression.RefreshVariables();
	AttackPowerExpression.RefreshVariables();
	SpellPowerExpression.RefreshVariables();
	AttackCritExpression.RefreshVariables();
	SpellCritExpression.RefreshVariables();
	HealthExpression.RefreshVariables();
	ManaExpression.RefreshVariables();
}

void UMMOStatsManager::RecomputeCombatAttributesChances()
{
	if (OwnerCharacter)
	{
		if (BlockExpression.IsValid())
		{
			BlockChance = BlockExpression.Eval<float>();
		}

		if (DodgeExpression.IsValid())
		{
			DodgeChance = DodgeExpression.Eval<float>();
		}

		if (AttackCritExpression.IsValid())
		{
			AttackCritChance = AttackCritExpression.Eval<float>();
		}

		if (SpellCritExpression.IsValid())
		{
			SpellCritChance = SpellCritExpression.Eval<float>();
		}

		if (AttackPowerExpression.IsValid())
		{
			AttackPowerValue = AttackPowerExpression.Eval<int32>();
		}

		if (SpellPowerExpression.IsValid())
		{
			SpellPowerValue = SpellPowerExpression.Eval<int32>();
		}
	}
}

void UMMOStatsManager::ComputeSecondaryAttributes()
{
	RecomputeCombatAttributes();
	RefreshMathExpressions();
	RecomputeCombatAttributesChances();
	RecomputeHealthAndResources();
}

void UMMOStatsManager::LevelUp()
{
	OwnerCharacter->CharacterInfo.Level++;

	RecomputeAttributes();

	OwnerCharacter->OnLevelUp();
}

