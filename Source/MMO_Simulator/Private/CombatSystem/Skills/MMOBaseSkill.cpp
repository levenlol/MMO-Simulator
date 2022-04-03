// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystem/Skills/MMOBaseSkill.h"
#include "Particles/ParticleSystemComponent.h"


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
	return Cast<UMMOWrapperSkill>(GetOuterSkill_Rec(this));
}

const UMMOBaseSkill* UMMOBaseSkill::GetOuterSkill_Rec(const UMMOBaseSkill* InSkill) const
{
	if (InSkill)
	{
		const UMMOBaseSkill* OuterSkill = Cast<UMMOBaseSkill>(InSkill->GetOuter());
		return OuterSkill ? GetOuterSkill_Rec(OuterSkill) : InSkill;
	}

	return nullptr;
}

AMMOFXActor::AMMOFXActor()
	: Super()
{

	ParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particles"));
	ParticleComponent->SetupAttachment(RootComponent);
}
