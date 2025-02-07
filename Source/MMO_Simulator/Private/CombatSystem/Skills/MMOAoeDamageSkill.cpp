// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystem/Skills/MMOAoeDamageSkill.h"
#include "Characters/MMOBaseCharacter.h"
#include "Core/MMOCommon.h"

UMMOAoeDamageSkill::UMMOAoeDamageSkill()
	: Super()
{
	CollisionChannel = ECollisionChannel::ECC_GameTraceChannel1;
}

void UMMOAoeDamageSkill::Setup(AMMOBaseCharacter* InOwner)
{
	Super::Setup(InOwner);

}

void UMMOAoeDamageSkill::CastAbility(const FMMOSkillInputData& Data)
{
	const FVector Location = Data.TargetActor ? Data.TargetActor->GetActorLocation() : Data.TargetLocation;

	TArray<FHitResult> Hits;
	if (GetWorld()->SweepMultiByChannel(Hits, Location, Location + FVector::UpVector, FQuat::Identity, CollisionChannel, FCollisionShape::MakeSphere(Radius)))
	{
		for (const FHitResult& Hit : Hits)
		{
			if (AMMOBaseCharacter* Character = Cast<AMMOBaseCharacter>(Hit.GetActor()))
			{
				FMMODamage MMODamage = ComputeDamage();
				OwnerCharacter->DoDamage(MMODamage, Character);
			}
		}
	}

	UMMOGameplayUtils::PlayParticlesAt(FxActor, Location);
}
