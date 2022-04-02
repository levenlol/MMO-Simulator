// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystem/Skills/MMOPoolDamageSkill.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Characters/MMOBaseCharacter.h"

AMMOPoolActor::AMMOPoolActor()
	: Super()
{
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collider"));
	RootComponent = SphereComponent;

	SphereComponent->SetCollisionProfileName(FName("ReactToEnemy"));

	ParticleComponent->SetupAttachment(RootComponent);

	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

UMMOPoolDamageSkill::UMMOPoolDamageSkill()
	: Super()
{
}

void UMMOPoolDamageSkill::Setup(AMMOBaseCharacter* InOwner)
{
	Super::Setup(InOwner);


	if (PoolClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = OwnerCharacter;
		Pool = GetWorld()->SpawnActor<AMMOPoolActor>(PoolClass, OwnerCharacter->GetActorLocation(), OwnerCharacter->GetActorRotation(), SpawnParams);

		SetPoolActive(false);
	}
}

void UMMOPoolDamageSkill::CastAbility(FMMOSkillInputData Data)
{
	Super::CastAbility(Data);

	if (!Pool)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot spawn Pool"));
		return;
	}

	// setup positions and other stuff
	UMMOGameplayUtils::PlayParticlesAt(Pool, Data.TargetActor ? Data.TargetActor->GetActorLocation() : Data.TargetLocation);
	SetPoolActive(true);

	// logic part
	CurrentTick = 0;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UMMOPoolDamageSkill::Tick, Duration / static_cast<float>(TickNumber), true, Duration / static_cast<float>(TickNumber));
}

void UMMOPoolDamageSkill::Tick()
{
	// if we are there Pool is valid.
	TArray<AActor*> OverlappingActors;
	Pool->SphereComponent->GetOverlappingActors(OverlappingActors, AMMOBaseCharacter::StaticClass());

	for(AActor* OverlappingActor : OverlappingActors)
	{
		AMMOBaseCharacter* Character = Cast<AMMOBaseCharacter>(OverlappingActor);
		for (UMMOBaseSkill* Skill : TriggeredSkills)
		{
			FMMOSkillInputData InputData;
			InputData.SourceActor = OwnerCharacter;
			InputData.SourceLocation = Pool->GetActorLocation();
			InputData.TargetActor = Character;
			InputData.TargetLocation = Character->GetActorLocation();

			Skill->CastAbility(InputData);
		}
	}

	if (++CurrentTick >= TickNumber)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		SetPoolActive(false);
	}
}

void UMMOPoolDamageSkill::SetPoolActive(const bool bActive)
{
	UMMOGameplayUtils::SetActorActive(Pool, bActive);
}
