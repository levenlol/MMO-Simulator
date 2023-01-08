// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystem/Skills/MMOAoeSkill.h"
#include "CombatSystem/MMOWrapperSkill.h"
#include "Characters/MMOBaseHero.h"
#include "Utils/MMOGameplayUtils.h"
#include "Characters/MMOBaseEnemy.h"

// Overrides the Setup function from the base class
// InOwner: The character that will be using this skill
void UMMOAoeSkill::Setup(AMMOBaseCharacter* InOwner)
{
	// Call the base class's Setup function
	Super::Setup(InOwner);

	// Setup each of the TriggeredSkills
	for (UMMOBaseSkill* Skill : TriggeredSkills)
	{
		Skill->Setup(InOwner);
	}

	// Set the MaxRange to the range of the outer skill
	MaxRange = GetOuterSkill()->Range;

	// Determine if the owner character is a hero or an enemy
	const bool bOwnerHero = OwnerCharacter->IsA<AMMOBaseHero>();

	// Determine if we should be targeting heroes or enemies based on the values of bOwnerHero and bTargetFriendly
	bTargetHeroes = bOwnerHero == bTargetFriendly;
}

// Overrides the CastAbility function from the base class
// Data: Input data for the skill
void UMMOAoeSkill::CastAbility(const FMMOSkillInputData& Data)
{
	// Call the base class's CastAbility function
	Super::CastAbility(Data);

	if (MaxRange <= 0.f)
	{
		// If MaxRange is not positive, return early
		return;
	}

	// Get the location of the owner character
	const FVector& PivotLocation = OwnerCharacter->GetActorLocation();

	// Get an array of all characters of the type specified by bTargetHeroes (either heroes or enemies)
	TArray<AMMOBaseCharacter*> Characters = UMMOGameplayUtils::GetAllCharactersOfType(this, bTargetHeroes ? AMMOBaseHero::StaticClass() : AMMOBaseEnemy::StaticClass());

	// Sort the array of characters by distance from the pivot location (closest first)
	Characters.Sort([PivotLocation](const AMMOBaseCharacter& First, const AMMOBaseCharacter& Second)
		{
			const float D1 = (First.GetActorLocation() - PivotLocation).SizeSquared();
			const float D2 = (Second.GetActorLocation() - PivotLocation).SizeSquared();

			return D1 <= D2;
		});

	// Initialize loop variables
	int32 i = 0;
	float CurrentDistance = (Characters[0]->GetActorLocation() - PivotLocation).Size();

	// Loop until we have hit the maximum number of units or the current distance exceeds the maximum range
	while (i < MaxUnitToHit && CurrentDistance < MaxRange)
	{
		// For each TriggeredSkill, create an input data object and use it to cast the skill on the current character in the loop
		for (UMMOBaseSkill* TriggeredSkill : TriggeredSkills)
		{
			FMMOSkillInputData InputData;
			InputData.SourceActor = OwnerCharacter;
			InputData.SourceLocation = PivotLocation;
			InputData.TargetActor = Characters[i];
			InputData.SourceLocation = Characters[i]->GetActorLocation();

		}

		// Increment the loop counter and update the current distance
		i++;
		CurrentDistance = (Characters[i]->GetActorLocation() - PivotLocation).Size();
	}
}
