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
	if (!InCharacter || !InCharacter->IsAlive() || InCharacter == GetOwner())
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

void UMMOAggroManagerComponent::Taunt(AMMOBaseCharacter* Attacker)
{
	if (!Attacker)
		return;

	if (AggroList.Num() == 0)
	{
		// nothing to do, just add it to the aggro list.
		AddDanger(Attacker, 0.f);
	}
	else
	{
		FMMOAggroData& TopDanger = AggroList.Last();
		if (TopDanger.Character == Attacker)
			return; // already on top aggro, nothing to do.

		// check if taunter is already on aggro list.
		float AggroToAdd = TopDanger.Aggro * 1.1f;
		FMMOAggroData* AggroData = AggroList.FindByPredicate([Attacker](const FMMOAggroData& AggroData) { return AggroData.Character == Attacker; });
		if (AggroData)
		{
			AggroToAdd -= AggroData->Aggro;
		}

		AddAggro(Attacker, AggroToAdd);

		SortAggroList();
	}
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
	const float Aggro = FMath::Max(static_cast<float>(Damage.Damage), 0.f) * DamageAggroPercent + FMath::Max(static_cast<float>(-Damage.Damage), 0.f) * HealAggroPercent;
	AddAggro(Sender, Aggro);

	SortAggroList();
}

void UMMOAggroManagerComponent::AddAggro(AMMOBaseCharacter* Danger, const float Aggro)
{
	if (Danger == GetOwner())
		return;

	FMMOAggroData* AggroData = AggroList.FindByPredicate([Danger](const FMMOAggroData& AggroData) { return AggroData.Character == Danger; });

	if (AggroData)
	{
		AggroData->Aggro += Aggro;
	}
	else
	{
		AggroList.Add(FMMOAggroData(Danger, Aggro));
	}
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
