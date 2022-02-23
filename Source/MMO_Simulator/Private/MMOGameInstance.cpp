// Fill out your copyright notice in the Description page of Project Settings.


#include "MMOGameInstance.h"
#include "Data/MMODataFinder.h"

void UMMOGameInstance::Init()
{
	Super::Init();

	UMMODataFinder::Startup(this);
}

void UMMOGameInstance::Shutdown()
{
	Super::Shutdown();

	UMMODataFinder::Shutdown();
}

UDataTable* UMMOGameInstance::RetrieveDataTable(FName Key) const
{
	if (DataTableMaps.Contains(Key))
	{
		return DataTableMaps[Key];
	}

	return nullptr;
}
