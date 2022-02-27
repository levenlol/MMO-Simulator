// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystem/MMOAoeDamageSkill.h"
#include "Characters/MMOBaseCharacter.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"

UMMOAoeDamageSkill::UMMOAoeDamageSkill()
	: Super()
{
	CollisionChannel = ECollisionChannel::ECC_GameTraceChannel1;
}

void UMMOAoeDamageSkill::CastAbility(FMMOSkillInputData Data)
{
	const FVector Location = Data.TargetActor ? Data.TargetActor->GetActorLocation() : Data.Location;

	TArray<FHitResult> Hits;
	if (GetWorld()->SweepMultiByChannel(Hits, Location, Location + FVector::UpVector, FQuat::Identity, CollisionChannel, FCollisionShape::MakeSphere(Radius)))
	{
		for (const FHitResult& Hit : Hits)
		{
			if (AMMOBaseCharacter* Character = Cast<AMMOBaseCharacter>(Hit.GetActor()))
			{
				FMMODamage MMODamage = ComputeDamage();
				Character->DamageTake(MMODamage);
			}
		}
	}

	// FX
	if(ParticleToPlay)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleToPlay, FTransform(FRotator::ZeroRotator, Location));
	}
}
