
#include "CombatSystem/MMOBeamSkill.h"
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

void UMMOBeamSkill::CastAbility(FMMOSkillInputData Data)
{
	Super::CastAbility(Data);

	if (Beam && Data.TargetActor && OwnerCharacter)
	{
		Beam->SetBeamSourceTarget(OwnerCharacter, Data.TargetActor);
		SetBeamActive(true);

		FTimerDelegate AttackDelegate;

		TWeakObjectPtr<UMMOBeamSkill> ThisWeak = this;

		AttackDelegate.BindLambda([ThisWeak, Data]()
			{
				UMMOBeamSkill* ThisStrong = ThisWeak.Get();
				if (!ThisStrong)
				{
					return;
				}

				ThisStrong->CurrentTick++;

				const FVector StartLocation = ThisStrong->OwnerCharacter->GetActorLocation();
				const FVector EndLocation = Data.TargetActor->GetActorLocation();

				const FVector BoxSize((StartLocation - EndLocation).Size() * 0.5f, 5.f, 5.f);
				const FQuat Rotation = (EndLocation - StartLocation).Rotation().Quaternion();

				const FVector MiddlePoint = (EndLocation + StartLocation) * 0.5f;

				//DrawDebugBox(ThisStrong->GetWorld(), MiddlePoint, BoxSize, Rotation, FColor::Magenta, false, 5.f, 0, 10.f);

				TArray<FHitResult> Hits;
				//if (ThisStrong->GetWorld()->LineTraceMultiByChannel(Hits, StartLocation, EndLocation, ThisStrong->CollisionChannel))
				if(ThisStrong->GetWorld()->SweepMultiByChannel(Hits, MiddlePoint, MiddlePoint + FVector::UpVector, Rotation, ThisStrong->CollisionChannel, FCollisionShape::MakeBox(BoxSize)))
				{
					for (const FHitResult& Hit : Hits)
					{
						FMMOSkillInputData BeamInput;
						BeamInput.Location = Hit.ImpactPoint;
						BeamInput.TargetActor = Cast<AMMOBaseCharacter>(Hit.GetActor());

						for (UMMOBaseSkill* Skill : ThisWeak->TriggeredSkills)
						{
							Skill->CastAbility(BeamInput);
						}
					}
				}

				if (ThisStrong->CurrentTick >= ThisStrong->TickNumber)
				{
					ThisStrong->GetWorld()->GetTimerManager().ClearTimer(ThisStrong->TimerHandle);
					ThisStrong->SetBeamActive(false);
				}

			});

		CurrentTick = 0;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, AttackDelegate, Duration / static_cast<float>(TickNumber), true, Duration / static_cast<float>(TickNumber));
	}
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
