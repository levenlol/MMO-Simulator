// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystem/Skills/MMOAoeDamageSkill.h"
#include "Characters/MMOBaseCharacter.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"


UMMOAoeDamageSkill::UMMOAoeDamageSkill()
	: Super()
{
	CollisionChannel = ECollisionChannel::ECC_GameTraceChannel1;
}

void UMMOAoeDamageSkill::Setup(AMMOBaseCharacter* InOwner)
{
	Super::Setup(InOwner);

	if (FxActorClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = OwnerCharacter;
		FxActor = GetWorld()->SpawnActor<AMMOFXActor>(FxActorClass, OwnerCharacter->GetActorLocation(), OwnerCharacter->GetActorRotation(), SpawnParams);
		FxActor->SetActorHiddenInGame(true);
	}
}

void UMMOAoeDamageSkill::CastAbility(FMMOSkillInputData Data)
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
				Character->DamageTake(MMODamage);
			}
		}
	}

	if (FxActor)
	{
		FxActor->SetActorLocation(Location);
		FxActor->SetActorHiddenInGame(false);

		FxActor->ParticleComponent->KillParticlesForced();
		FxActor->ParticleComponent->ResetParticles();
		FxActor->ParticleComponent->ActivateSystem();
	}
}
