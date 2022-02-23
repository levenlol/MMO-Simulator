// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MMOAggroManagerComponent.h"
#include "Characters/MMOBaseCharacter.h"

UMMOAggroManagerComponent::UMMOAggroManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
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
	if (AggroData)
	{
		AggroData->Aggro += fabsf(static_cast<float>(Damage.Damage)) * DamageAggroPercent;
	}
	else
	{
		AggroList.Add(FMMOAggroData(Damage.DamageDealer, static_cast<float>(Damage.Damage)));
	}

	AggroList.Sort();
}


void UMMOAggroManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

