// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystem/Skills/MMOMeleeSkill.h"
#include "Characters/MMOBaseCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimationAsset.h"

void UMMOMeleeSkill::Setup(AMMOBaseCharacter* InOwner)
{
	Super::Setup(InOwner);
	

}

void UMMOMeleeSkill::CastAbility(FMMOSkillInputData Data)
{
	Super::CastAbility(Data);

	if (IsValid(OwnerCharacter) && AnimationAsset)
	{
		USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh();
		Mesh->PlayAnimation(AnimationAsset, false);

		const float AnimLength = AnimationAsset->GetPlayLength();

		FTimerHandle UnusuedHandle;
		FTimerDelegate AttackDelegate;

		TWeakObjectPtr<UMMOMeleeSkill> ThisWeak = this;
		TWeakObjectPtr<USkeletalMeshComponent> ThisMesh = Mesh;

		AttackDelegate.BindLambda([ThisWeak, ThisMesh, Data]()
			{
				if (!ThisWeak.IsValid() || !ThisMesh.IsValid())
				{
					return;
				}

				ThisMesh->Stop();
				ThisMesh->SetAnimationMode(EAnimationMode::Type::AnimationBlueprint);
				for (UMMOBaseSkill* Skill : ThisWeak->TriggeredSkills)
				{
					Skill->CastAbility(Data);
				}
			});

		GetWorld()->GetTimerManager().SetTimer(UnusuedHandle, AttackDelegate, AnimLength, false, AnimLength);
	}
}
