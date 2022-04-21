#include "Bosses/MMOBossManagerComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "AI/MMOAggroManagerComponent.h"

UMMOBossManagerComponent::UMMOBossManagerComponent()
	: Super()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}


void UMMOBossManagerComponent::OnDropHealthEvent_Implementation(const FMMOHealthPercentEventData& HealthPercentData)
{
	if (HealthPercentData.NewBT)
	{
		GetAIController()->StopBehaviorTree(FString("New Phase."));
		GetAIController()->StartBehaviorTree(HealthPercentData.NewBT);
	}
}

void UMMOBossManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (DropHealthPercentEvents.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Boss: %s doesn't have any phases"), *GetOwner()->GetName());
		return;
	}

	if (DropHealthPercentEvents[0].HealthPercent != 100)
	{
		UE_LOG(LogTemp, Warning, TEXT("Boss: %s first phase isn't set to start at 100% HP"), *GetOwner()->GetName());
	}

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
	if (DropHealthPercentEvents.Num() > 0)
	{
		CurrentPhase = 0;
		OnDropHealthEvent(DropHealthPercentEvents[0]);
	
		SetComponentTickEnabled(true);
	}
}

void UMMOBossManagerComponent::CheckPhase()
{
	const int32 BossPercent = GetBossHealthPercent();

	int32 Phase = -1;
	for (int32 i = 0; i < DropHealthPercentEvents.Num() - 1; i++)
	{
		const bool bOverMin = BossPercent <= DropHealthPercentEvents[i].HealthPercent;
		const bool bUnderMax = BossPercent >= DropHealthPercentEvents[i].HealthPercent;

		if (bOverMin && bUnderMax)
		{
			Phase = i;
			break;
		}
	}

	if (Phase >= 0 && Phase != CurrentPhase)
	{
		CurrentPhase = Phase;
		OnDropHealthEvent(DropHealthPercentEvents[CurrentPhase]);
	}
}

void UMMOBossManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CheckPhase();
}

