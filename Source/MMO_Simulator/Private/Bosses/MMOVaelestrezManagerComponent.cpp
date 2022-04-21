#include "Bosses/MMOVaelestrezManagerComponent.h"
#include "CombatSystem/MMOCombatSystem.h"

UMMOVaelestrezManagerComponent::UMMOVaelestrezManagerComponent()
	: Super()
{
}


void UMMOVaelestrezManagerComponent::OnDropHealthEvent_Implementation(const FMMOHealthPercentEventData& HealthPercentData)
{
	Super::OnDropHealthEvent_Implementation(HealthPercentData);

	if (HealthPercentData.HealthPercent == 80)
	{
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

