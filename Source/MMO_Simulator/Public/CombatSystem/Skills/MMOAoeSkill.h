/**

@file MMOAoeSkill.h
@brief A skill that affects an area around the caster.
This file contains the declaration of the UMMOAoeSkill class, a subclass of UMMOBaseSkill that represents a skill that affects an area around the caster.
*/

#pragma once

#include "CoreMinimal.h"
#include "CombatSystem/Skills/MMOBaseSkill.h"
#include "MMOAoeSkill.generated.h"


/**

UMMOAoeSkill is a subclass of UMMOBaseSkill that represents a skill that affects an area around the caster.
It has the following additional properties:
MaxUnitToHit: The maximum number of units that the skill will hit.
bTargetFriendly: Whether the skill will target friendly units or enemy units.
The UMMOAoeSkill class overrides the Setup() and CastAbility() methods of UMMOBaseSkill to add functionality specific to area of effect skills.
*/

USTRUCT()
struct MMO_SIMULATOR_API FMMOTriggeredSkills
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TArray<UMMOBaseSkill*> TriggeredSkills;
};

UCLASS()
class MMO_SIMULATOR_API UMMOAoeSkill : public UMMOBaseSkill
{
	GENERATED_BODY()
public:
	/**
	* Sets up the skill for use.
	* This includes setting up any triggered skills, as well as calculating the maximum range of the skill based on the Range property of the outer skill.
	* @param InOwner The character that owns this skill.
	*/
	virtual void Setup(AMMOBaseCharacter* InOwner) override;

	/**
	 * Casts the skill.
	 * This includes finding all characters within the maximum range of the skill, sorting them based on distance from the caster, and applying the triggered skills to the first MaxUnitToHit units.
	 * @param Data The input data for the skill.
	 */
	virtual void CastAbility(const FMMOSkillInputData& Data) override;

	/**
	 * The maximum number of units that the skill will hit.
	 */
	UPROPERTY(EditAnywhere, Category = Skill, meta=(UIMin = "1", ClampMin="1"))
	int32 MaxUnitToHit = 5;

	/**
	 * Whether the skill will target friendly units or enemy units.
	 * True means friendly units, false means enemy units.
	 */
	UPROPERTY(EditAnywhere, Category = Skill)
	bool bTargetFriendly = true;

private:
	/**
	* The maximum range of the skill. It is derived from the base wrapped-skill
	*/
	UPROPERTY(VisibleAnywhere, Category = Skill)
	float MaxRange = 0.f;

	bool bTargetHeroes = true;

	UPROPERTY()
	TArray<FMMOTriggeredSkills> DuplicatedTriggeredSkills;
};
