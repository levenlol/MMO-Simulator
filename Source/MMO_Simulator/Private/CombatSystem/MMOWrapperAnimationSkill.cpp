// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystem/MMOWrapperAnimationSkill.h"
#include "Characters/MMOBaseCharacter.h"
#include "Components/SkeletalMeshComponent.h"

void UMMOWrapperAnimationSkill::TryCastAbility(const FMMOSkillInputData& Data)
{
	if (CastTime <= 0.f && ChannelingTime <= 0.f)
	{
		SavedInputData = Data;

		// do damage and all skill logic OnAnimationEnd
		if (IsValid(OwnerCharacter) && AnimationAsset)
		{
			USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh();
			Mesh->PlayAnimation(AnimationAsset, false);

			const float AnimLength = AnimationAsset->GetPlayLength();

			GetWorld()->GetTimerManager().SetTimer(AnimationHandle, this, &UMMOWrapperAnimationSkill::OnAnimationEnd, AnimLength, false, AnimLength);
		}
	}
	else
	{
		// animation is just a custom cast animation.
		Super::TryCastAbility(Data);
	}
}

void UMMOWrapperAnimationSkill::AbortAbility()
{
	Super::AbortAbility();

	GetWorld()->GetTimerManager().ClearTimer(AnimationHandle);
	StopAnimation();
}

void UMMOWrapperAnimationSkill::FinishCastAbility()
{
	Super::FinishCastAbility();

	if (CastTime > 0.f && ChannelingTime <= 0.f)
	{
		StopAnimation();
	}
}

void UMMOWrapperAnimationSkill::OnAnimationEnd()
{
	Super::TryCastAbility(SavedInputData); // do damage.

	StopAnimation();
}

void UMMOWrapperAnimationSkill::StopAnimation()
{
	if (IsValid(OwnerCharacter) && AnimationAsset)
	{
		USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh();
		Mesh->SetAnimationMode(EAnimationMode::Type::AnimationBlueprint);
		Mesh->Stop();
	}
}
