// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MMOAggroManagerComponent.h"
#include "Characters/MMOBaseCharacter.h"

UMMOAggroManagerComponent::UMMOAggroManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMMOAggroManagerComponent::AddDanger(AMMOBaseCharacter* InCharacter, float AggroBoost /*= 0.f*/)
{
	if (!InCharacter)
		return;

	
	if (!AggroList.FindByPredicate([InCharacter](const FMMOAggroData& Data) { return Data.Character == InCharacter; }))
	{
		AggroList.Emplace(InCharacter, AggroOnPull);
	}

	SortAggroList();
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

}

