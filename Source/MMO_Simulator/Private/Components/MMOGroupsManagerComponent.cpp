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
	Groups.FindOrAdd(InName) = MoveTemp(Group);
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

