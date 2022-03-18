// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/MMOStatsManager.h"
#include "Characters/MMOBaseCharacter.h"
#include "Data/MMODataFinder.h"

UMMOStatsManager::UMMOStatsManager()
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
}

void UMMOStatsManager::OnCharacterInitialized(AMMOBaseCharacter* Sender)
{
	check(Sender == OwnerCharacter);

	ParryExpression.Init(this);
	BlockExpression.Init(this);
	DodgeExpression.Init(this);
	AttackCritExpression.Init(this);
	SpellCritExpression.Init(this);
	AttackPowerExpression.Init(this);
	SpellPowerExpression.Init(this);
	HealthExpression.Init(this);
	ManaExpression.Init(this);

	ComputeAttributes();
}

void UMMOStatsManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void UMMOStatsManager::OnRegister()
{
	Super::OnRegister();

	OwnerCharacter = Cast<AMMOBaseCharacter>(GetOwner());

	if (OwnerCharacter)
	{
		OwnerCharacter->OnCharacterInitialized.AddUniqueDynamic(this, &UMMOStatsManager::OnCharacterInitialized);
	}
}

void UMMOStatsManager::OnUnregister()
{
	Super::OnUnregister();

	if (OwnerCharacter)
	{
		OwnerCharacter->OnCharacterInitialized.RemoveDynamic(this, &UMMOStatsManager::OnCharacterInitialized);
	}


	ParryExpression.Release();
	BlockExpression.Release();
	DodgeExpression.Release();
	AttackCritExpression.Release();
	SpellCritExpression.Release();
	AttackPowerExpression.Release();
	SpellPowerExpression.Release();
	HealthExpression.Release();
	ManaExpression.Release();
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
		// TODO: sum equip stats.
		FMMOCharacterAttributes FullAttributes = BaseAttributes;
		// ... sum equip attributes...

		OwnerCharacter->CharacterInfo.Attributes = FullAttributes;

		// recompute all other stuff
		ComputeAttributes();
	}
}

void UMMOStatsManager::RecomputeCombatAttributesChances()
{
	if (OwnerCharacter)
	{
		if (ParryExpression.IsValid())
		{
			ParryChance = ParryExpression.Eval<float>();
		}

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

void UMMOStatsManager::ComputeAttributes()
{
	RecomputeCombatAttributesChances();
	RecomputeHealthAndResources();
}

void UMMOStatsManager::LevelUp()
{
	// Compute new base stats
	const FMMOCharacterAttributes& InitialAttributes = OwnerCharacter->CharacterInfo.GetInitialAttributes();
	OwnerCharacter->CharacterInfo.Level++;

	const FMMOCharacterAttributes ProgressionAttributes = UMMODataFinder::Get()->GetCharacterProgressionAtLevel(OwnerCharacter->CharacterInfo.CharacterClass, OwnerCharacter->CharacterInfo.Level);

	BaseAttributes = InitialAttributes + ProgressionAttributes;
	UpdateCharacterAttributes();
}

