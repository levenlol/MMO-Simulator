#include "Bosses/MMOVaelestrezManagerComponent.h"
#include "CombatSystem/MMOCombatSystem.h"
#include "Utils/MMOGameplayUtils.h"
#include "Characters/MMOBaseBoss.h"

UMMOVaelestrezManagerComponent::UMMOVaelestrezManagerComponent()
	: Super()
{
}

void UMMOVaelestrezManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	AddCustomBossEvent(this, &UMMOVaelestrezManagerComponent::DeathNova_Condition<80>, &UMMOVaelestrezManagerComponent::DeathNova_Callback);
	AddCustomBossEvent(this, &UMMOVaelestrezManagerComponent::DeathNova_Condition<60>, &UMMOVaelestrezManagerComponent::DeathNova_Callback);
	AddCustomBossEvent(this, &UMMOVaelestrezManagerComponent::DeathNova_Condition<40>, &UMMOVaelestrezManagerComponent::DeathNova_Callback);
	AddCustomBossEvent(this, &UMMOVaelestrezManagerComponent::DeathNova_Condition<20>, &UMMOVaelestrezManagerComponent::DeathNova_Callback);
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
			Location = UMMOGameplayUtils::VerticalRaycast(this, Location, 400.f, TerrainCollisionChannel);

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

bool UMMOVaelestrezManagerComponent::DeathNova_Condition(AMMOBaseBoss* Boss, int32 HealthPercent)
{
	return Boss && FMath::RoundToInt(Boss->GetHealthPercent() * 100.f) <= HealthPercent;
}

void UMMOVaelestrezManagerComponent::DeathNova_Callback(AMMOBaseBoss* Boss)
{
	SpawnSafetyPillars();

	const int32 SpellIndex = Boss->CombatSystem->AddSkill(DeathNovaSkill);
	if (AMMOAIController* AIController = GetAIController())
	{
		AIController->AddPlayerHandledAbility(SpellIndex);
	}

	GetBossPawn()->CombatSystem->TryCastSkill(Boss, Boss->GetActorLocation(), SpellIndex);
}

