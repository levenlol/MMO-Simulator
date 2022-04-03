
#include "CombatSystem/Skills/MMOBeamSkill.h"
#include "Particles/ParticleSystemComponent.h"
#include "Characters/MMOBaseCharacter.h"
#include "Core/MMOCommon.h"

UMMOBeamSkill::UMMOBeamSkill()
	: Super()
{
	CollisionChannel = ECollisionChannel::ECC_GameTraceChannel1;
}

void UMMOBeamSkill::Setup(AMMOBaseCharacter* InOwner)
{
	Super::Setup(InOwner);

	if (BeamClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = OwnerCharacter;
		Beam = GetWorld()->SpawnActor<AMMOBeam>(BeamClass, OwnerCharacter->GetActorLocation(), OwnerCharacter->GetActorRotation(), SpawnParams);

		SetBeamActive(false);
	}
}

void UMMOBeamSkill::CastAbility(const FMMOSkillInputData& Data)
{
	Super::CastAbility(Data);

	if (Beam && Data.TargetActor && OwnerCharacter)
	{
		Beam->SetBeamSourceTarget(OwnerCharacter, Data.TargetActor);
		SetBeamActive(true);

		const FVector StartLocation = OwnerCharacter->GetActorLocation();
		const FVector EndLocation = Data.TargetActor->GetActorLocation();

		const FVector BoxSize((StartLocation - EndLocation).Size() * 0.5f, 5.f, 5.f);
		const FQuat Rotation = (EndLocation - StartLocation).Rotation().Quaternion();

		const FVector MiddlePoint = (EndLocation + StartLocation) * 0.5f;

		TArray<FHitResult> Hits;
		if (GetWorld()->SweepMultiByChannel(Hits, MiddlePoint, MiddlePoint + FVector::UpVector, Rotation, CollisionChannel, FCollisionShape::MakeBox(BoxSize)))
		{
			for (const FHitResult& Hit : Hits)
			{
				FMMOSkillInputData BeamInput = Data;
				BeamInput.TargetLocation = Hit.ImpactPoint;
				BeamInput.TargetActor = Cast<AMMOBaseCharacter>(Hit.GetActor());

				for (UMMOBaseSkill* Skill : TriggeredSkills)
				{
					Skill->CastAbility(BeamInput);
				}
			}
		}
	}
}

void UMMOBeamSkill::Finish()
{
	SetBeamActive(false);
}

void UMMOBeamSkill::Abort()
{
	SetBeamActive(false);
}

void UMMOBeamSkill::SetBeamActive(bool bActive)
{
	UMMOGameplayUtils::SetActorActive(Beam, bActive);
}

AMMOBeam::AMMOBeam()
	: Super()
{
	ParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Beam"));
	RootComponent = ParticleComponent;
}

void AMMOBeam::SetBeamSourceTarget(AActor* InSource, AActor* InTarget)
{
	Source = InSource;
	Target = InTarget;

	if (Source && Target)
	{
		ParticleComponent->SetActorParameter(FName("Source"), Source);
		ParticleComponent->SetActorParameter(FName("Target"), Target);
	}
}

void AMMOBeam::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}
