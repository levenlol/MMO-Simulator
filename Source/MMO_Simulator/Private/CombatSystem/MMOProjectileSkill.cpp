// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystem/MMOProjectileSkill.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Characters/MMOBaseCharacter.h"

AMMOProjectile::AMMOProjectile()
	: Super()
{
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collider"));
	RootComponent = SphereComponent;

	SphereComponent->SetCollisionProfileName(FName("ReactToEnemy"));

	ParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particles"));
	ParticleComponent->SetupAttachment(RootComponent);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void AMMOProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(!Target)
	{
		return;
	}

	const FVector Location = GetActorLocation();
	const FVector Direction = (Target->GetActorLocation() - Location).GetSafeNormal();
	
	const FVector NewLocation = Location + Direction * Speed * DeltaSeconds;
	SetActorLocationAndRotation(NewLocation, Direction.Rotation());
}


void UMMOProjectileSkill::Setup(AMMOBaseCharacter* InOwner)
{
	Super::Setup(InOwner);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = OwnerCharacter;
	Projectile = GetWorld()->SpawnActor<AMMOProjectile>(ProjectileClass, OwnerCharacter->GetActorLocation(), OwnerCharacter->GetActorRotation(), SpawnParams);
	Projectile->SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &UMMOProjectileSkill::OnProjectileOverlap);

	SetProjectileActive(false);
}

void UMMOProjectileSkill::CastAbility(FMMOSkillInputData Data)
{
	Super::CastAbility(Data);

	if (!Data.TargetActor || !ProjectileClass)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot spawn projectile"));
		return;
	}

	Projectile->SetTarget(Data.TargetActor);
	Projectile->SetActorLocation(OwnerCharacter->GetActorLocation());
	Projectile->ParticleComponent->KillParticlesForced();
	SetProjectileActive(true);
}

void UMMOProjectileSkill::OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == Projectile->GetTarget())
	{
		for (UMMOBaseSkill* Skill : TriggeredSkills)
		{
			FMMOSkillInputData Data;
			Data.TargetActor = OtherActor;
			
			Skill->CastAbility(Data);
		}

		SetProjectileActive(false);
	}
}

void UMMOProjectileSkill::SetProjectileActive(bool bActive)
{
	Projectile->SetActorHiddenInGame(!bActive);
	Projectile->SetActorEnableCollision(bActive);
	Projectile->SetActorTickEnabled(bActive);
}
