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
	BaseCharacterInfo = OwnerCharacter->CharacterInfo;

	// if Character has an higher level we need to compute the base stats.
	if (BaseCharacterInfo.Level > 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Character Has been initialized with a level > 1. Automatic Fixing MMOStatsManager"));
		FMMOCharacterAttributes Attributes = BaseCharacterInfo.Attributes;
		const FMMOCharacterAttributes ProgressionAttributes = UMMODataFinder::Get()->GetCharacterProgressionAtLevel(BaseCharacterInfo.CharacterClass, BaseCharacterInfo.Level);

		Attributes.Strength -= ProgressionAttributes.Strength;
		Attributes.Intellect -= ProgressionAttributes.Intellect;
		Attributes.Constitution -= ProgressionAttributes.Constitution;
		Attributes.Dexterity -= ProgressionAttributes.Dexterity;

		// this is the base stats
		BaseCharacterInfo.Attributes = Attributes;
		BaseCharacterInfo.Level = 1;
	}

	RecomputeCombatAttributesChances();
}

void UMMOStatsManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void UMMOStatsManager::InitializeComponent()
{
	Super::InitializeComponent();

	OwnerCharacter = Cast<AMMOBaseCharacter>(GetOwner());

	if (OwnerCharacter)
	{
		OwnerCharacter->OnCharacterInitialized.AddDynamic(this, &UMMOStatsManager::OnCharacterInitialized);
	}

	ParryExpression.Init(this);
	BlockExpression.Init(this);
	DodgeExpression.Init(this);
	AttackCritExpression.Init(this);
	SpellCritExpression.Init(this);
}

void UMMOStatsManager::UninitializeComponent()
{
	Super::UninitializeComponent();

	if (OwnerCharacter)
	{
		OwnerCharacter->OnCharacterInitialized.RemoveDynamic(this, &UMMOStatsManager::OnCharacterInitialized);
	}
}


void UMMOStatsManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UMMOStatsManager::UpdateCharacterAttributes(const FMMOCharacterAttributes& InAttributes, const FMMOCombatAttributes& InCombatAttributes)
{
	if (OwnerCharacter)
	{
		OwnerCharacter->CharacterInfo.Attributes = InAttributes;
		OwnerCharacter->CharacterInfo.CombatAttributes = InCombatAttributes;
		
		RecomputeCombatAttributesChances();
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
	}
}

void UMMOStatsManager::OnLevelUp()
{
	// todo
}

