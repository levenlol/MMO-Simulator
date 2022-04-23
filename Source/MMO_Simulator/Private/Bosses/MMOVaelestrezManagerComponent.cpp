#include "Bosses/MMOVaelestrezManagerComponent.h"
#include "CombatSystem/MMOCombatSystem.h"
#include "Core/MMOCommon.h"

UMMOVaelestrezManagerComponent::UMMOVaelestrezManagerComponent()
	: Super()
{
}


void UMMOVaelestrezManagerComponent::OnDropHealthEvent_Implementation(const FMMOHealthPercentEventData& HealthPercentData)
{
	Super::OnDropHealthEvent_Implementation(HealthPercentData);

	if (HealthPercentData.HealthPercent == 80)
	{
		SpawnSafetyPillars();

		AMMOBaseBoss* Boss = GetBossPawn();

		const int32 SpellIndex = Boss->CombatSystem->AddSkill(DeathNovaSkill);
		if (AMMOAIController* AIController = GetAIController())
		{
			AIController->AddPlayerHandledAbility(SpellIndex);
		}

		GetBossPawn()->CombatSystem->TryCastSkill(Boss, Boss->GetActorLocation(), SpellIndex);
	}
}

void UMMOVaelestrezManagerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMMOVaelestrezManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UMMOVaelestrezManagerComponent::SpawnSafetyPillars()
{
	if (PillarsClass && NumberOfPillars > 0)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		const float Step = PI / NumberOfPillars * 2.f;
		float CurrentAngle = 0.f;
		for (int32 i = 0; i < NumberOfPillars; i++)
		{
			// Compute Angle to spawn
			const float SinVal = FMath::Sin(CurrentAngle);
			const float CosVal = FMath::Cos(CurrentAngle);

			const FVector Direction(CosVal, SinVal, 0.f);

			FVector Location = GetBossPawn()->GetActorLocation() + Direction * RockDisplacementLength;
			Location = UMMOGameplayUtils::ProjectPointToTerrain(this, Location, 400.f, TerrainCollisionChannel);

			AActor* SpawnedPillar = GetWorld()->SpawnActor<AActor>(PillarsClass, Location, FRotator::ZeroRotator, SpawnParams);
			check(SpawnedPillar);
			SpawnedPillars.Add(SpawnedPillar);

			CurrentAngle += Step;
		}

		FTimerHandle Unusued;
		TWeakObjectPtr<UMMOVaelestrezManagerComponent> ThisWeak = this;
		GetWorld()->GetTimerManager().SetTimer(Unusued, [ThisWeak]() 
			{
				if (ThisWeak.IsValid())
				{
					TArray<AActor*>& Pillars = ThisWeak->SpawnedPillars;
					for (AActor* Pillar : Pillars)
					{
						if (Pillar)
							Pillar->Destroy();
					}

					Pillars.Empty();
				}
			}, PillarsDuration, false, PillarsDuration);
	}
}

