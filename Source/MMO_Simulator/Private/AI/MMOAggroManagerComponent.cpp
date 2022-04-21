// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MMOAggroManagerComponent.h"
#include "Characters/MMOBaseCharacter.h"
#include "Core/MMOCommon.h"


UMMOAggroManagerComponent::UMMOAggroManagerComponent()
	: Super()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMMOAggroManagerComponent::AddDanger(AMMOBaseCharacter* InCharacter, float AggroBoost /*= 0.f*/)
{
	if (!InCharacter || !InCharacter->IsAlive())
		return;
	
	if (!AggroList.FindByPredicate([InCharacter](const FMMOAggroData& Data) { return Data.Character == InCharacter; }))
	{
		AggroList.Emplace(InCharacter, AggroList.Num() <= 0 ? AggroOnPull : 0.f);

		if (AggroList.Num() == 1)
			EnterCombatDelegate.Broadcast(this, InCharacter);
	}

	SortAggroList();
}

AMMOBaseCharacter* UMMOAggroManagerComponent::GetMostDangerousCharacter() const
{
	if (AggroList.Num() > 0)
	{
		return AggroList.Last().Character;
	}

	return nullptr;
}

void UMMOAggroManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	AMMOBaseCharacter* OwnerCharacter = Cast<AMMOBaseCharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("AggroManagerComponent should be attached to a MMOBaseCharacter."));
		return;
	}

	OwnerCharacter->OnDamageTaken.AddDynamic(this, &UMMOAggroManagerComponent::OnTakeDamage);
}

void UMMOAggroManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	AMMOBaseCharacter* OwnerCharacter = Cast<AMMOBaseCharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		return;
	}

	OwnerCharacter->OnDamageTaken.RemoveDynamic(this, &UMMOAggroManagerComponent::OnTakeDamage);
}

void UMMOAggroManagerComponent::OnTakeDamage(AMMOBaseCharacter* Sender, FMMODamage Damage)
{
	FMMOAggroData* AggroData = AggroList.FindByPredicate([DamageDealer = Damage.DamageDealer](const FMMOAggroData& AggroData) { return AggroData.Character == DamageDealer; });

	const float Aggro = FMath::Max(static_cast<float>(Damage.Damage), 0.f) * DamageAggroPercent + FMath::Max(static_cast<float>(-Damage.Damage), 0.f) * HealAggroPercent;
	if (AggroData)
	{
		AggroData->Aggro += Aggro;
	}
	else
	{
		AggroList.Add(FMMOAggroData(Damage.DamageDealer, Aggro));
	}

	SortAggroList();
}

void UMMOAggroManagerComponent::SortAggroList()
{
	if (AggroList.Num() > 1)
	{
		AggroList.Sort([StealAggroThreshold = PercentToStealAggro, Top = AggroList[0].Character](const FMMOAggroData& A, const FMMOAggroData& B)
		{
			if (Top == A.Character)
				return  A.Aggro < B.Aggro * StealAggroThreshold;

			return A < B;
		});
	}
}

void UMMOAggroManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

#if WITH_EDITORONLY_DATA
	if (bDebug)
	{
		for (int32 i = 0; i < AggroList.Num(); i++)
		{
			const FMMOAggroData& Aggro = AggroList[i];

			const auto& Info = Aggro.Character->CharacterInfo;
			GEngine->AddOnScreenDebugMessage(1000 + i, 0.1f, FColor::Blue, FString::Printf(TEXT("%d - %s - %s - Aggro: %f"), i, *UMMOGameplayUtils::GetClassName(Info.CharacterClass), *Info.Name.ToString(), Aggro.Aggro));
		}
	}
#endif
}
