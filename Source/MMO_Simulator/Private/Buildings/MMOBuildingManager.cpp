// Fill out your copyright notice in the Description page of Project Settings.


#include "Buildings/MMOBuildingManager.h"

UMMOBuildingManager::UMMOBuildingManager()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMMOBuildingManager::BeginPlay()
{
	Super::BeginPlay();
}

void UMMOBuildingManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

