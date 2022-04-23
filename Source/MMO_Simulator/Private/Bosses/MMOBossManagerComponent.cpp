#include "Bosses/MMOBossManagerComponent.h"
#include "AI/MMOAggroManagerComponent.h"

UMMOBossManagerComponent::UMMOBossManagerComponent()
	: Super()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UMMOBossManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentPhase = 0;

	GetBossPawn()->AggroManager->EnterCombatDelegate.AddDynamic(this, &UMMOBossManagerComponent::OnEnterCombat);
}

int32 UMMOBossManagerComponent::GetBossHealthPercent() const
{
	const float Health = GetBossPawn()->GetHealthPercent();
	return FMath::FloorToInt(Health * 100.f);
}

void UMMOBossManagerComponent::OnEnterCombat(UMMOAggroManagerComponent* Sender, AMMOBaseCharacter* Danger)
{
	CheckEvents();

	SetComponentTickEnabled(true);
}

void UMMOBossManagerComponent::CheckEvents()
{
	AMMOBaseBoss* Boss = GetBossPawn();
	
	// Custom Events
	for (int32 i = PendingEvents.Num() - 1; i >= 0; i--)
	{
		FMMOEventDelegateHelper& PendingEvent = PendingEvents[i];

		if (PendingEvent.Execute_Condition(Boss))
		{
			PendingEvent.Execute_Callback(Boss);
			PendingEvents.RemoveAt(i);
		}
	}
}

void UMMOBossManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CheckEvents();
}

