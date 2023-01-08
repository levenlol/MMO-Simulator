// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystem/Skills/MMOBaseSkill.h"
#include "Particles/ParticleSystemComponent.h"
#include "CombatSystem/MMOWrapperSkill.h"


FMMOSkillTags FMMOSkillTags::SkillTags;

void UMMOBaseSkill::Setup(AMMOBaseCharacter* InOwner)
{
	OwnerCharacter = InOwner;

	for (UMMOBaseSkill* Skill : TriggeredSkills)
	{
		Skill->Setup(InOwner);
	}
}

const UMMOWrapperSkill* UMMOBaseSkill::GetOuterSkill() const
{
	return GetOuterSkill_Rec(this);
}

const UMMOWrapperSkill* UMMOBaseSkill::GetOuterSkill_Rec(const UObject* InSkill) const
{
	if (InSkill)
	{
		const UObject* OuterSkill = InSkill->GetOuter();
		return OuterSkill->IsA<UMMOWrapperSkill>() ? Cast<UMMOWrapperSkill>(OuterSkill) : GetOuterSkill_Rec(OuterSkill);
	}

	return nullptr;
}

AMMOFXActor::AMMOFXActor()
	: Super()
{

	ParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particles"));
	ParticleComponent->SetupAttachment(RootComponent);
}
