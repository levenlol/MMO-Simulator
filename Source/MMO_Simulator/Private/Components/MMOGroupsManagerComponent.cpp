// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/MMOGroupsManagerComponent.h"
#include "Characters/MMOBaseHero.h"

UMMOGroupsManagerComponent::UMMOGroupsManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMMOGroupsManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UMMOGroupsManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UMMOGroupsManagerComponent::AddGroup(FName InName, FMMOGroup Group)
{
	if (Group.Heroes.Num() > 0)
	{
		Groups.FindOrAdd(InName) = MoveTemp(Group);

		// Broadcast event.
		OnGroupCreatedOrModified.Broadcast(InName);
	}
}

const FMMOGroup& UMMOGroupsManagerComponent::GetGroup(FName Name) const
{
	if (HasGroup(Name))
	{
		return Groups[Name];
	}

	static FMMOGroup EmptyGroup;
	return EmptyGroup;
}

TArray<FName> UMMOGroupsManagerComponent::GetGroupNames() const
{
	TArray<FName> Names;
	Groups.GenerateKeyArray(Names);

	return Names;
}

