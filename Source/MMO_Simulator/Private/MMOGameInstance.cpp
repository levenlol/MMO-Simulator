// Fill out your copyright notice in the Description page of Project Settings.


#include "MMOGameInstance.h"
#include "Data/MMODataFinder.h"
#include "Core/MMOGuildsManager.h"
#include "Kismet/GameplayStatics.h"

void UMMOGameInstance::Init()
{
	Super::Init();

	UMMODataFinder::Startup(this);

	GuildsManager = NewObject<UMMOGuildsManager>(this, GuildsManagerClass ? GuildsManagerClass : UMMOGuildsManager::StaticClass());
	GuildsManager->Init();
}

void UMMOGameInstance::Shutdown()
{
	Super::Shutdown();

	UMMODataFinder::Shutdown();
	
	GuildsManager->Uninit();
	GuildsManager = nullptr;
}

UMMOGameInstance* UMMOGameInstance::GetMMOGameInstance(const UObject* WorldContextObject)
{
	return Cast<UMMOGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
}

UDataTable* UMMOGameInstance::RetrieveDataTable(FName Key) const
{
	if (DataTableMaps.Contains(Key))
	{
		return DataTableMaps[Key];
	}

	return nullptr;
}
