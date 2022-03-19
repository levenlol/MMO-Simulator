// Fill out your copyright notice in the Description page of Project Settings.


#include "MMOGameInstance.h"
#include "Data/MMODataFinder.h"
#include "Guilds/MMOGuildsManager.h"
#include "Kismet/GameplayStatics.h"
#include "Data/MMOCharacterGenerator.h"
#include "Data/MMOEquipGenerator.h"


void UMMOGameInstance::Init()
{
	Super::Init();

	UMMODataFinder::Startup(this);
	UMMOCharacterGenerator::Startup(this);
	UMMOEquipGenerator::Startup(this);

	GuildsManager = NewObject<UMMOGuildsManager>(this, GuildsManagerClass ? GuildsManagerClass : UMMOGuildsManager::StaticClass());
	GuildsManager->Init();
}

void UMMOGameInstance::Shutdown()
{
	Super::Shutdown();

	UMMODataFinder::Shutdown();
	UMMOCharacterGenerator::Shutdown();
	UMMOEquipGenerator::Shutdown();
	
	GuildsManager->Uninit();
	GuildsManager = nullptr;
}

UMMOGameInstance* UMMOGameInstance::GetMMOGameInstance(const UObject* WorldContextObject)
{
	return Cast<UMMOGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
}

void UMMOGameInstance::AddCharacterForRaid(const FMMOCharacter& InCharacter)
{
	RaidCharacters.Add(InCharacter);
}

UDataTable* UMMOGameInstance::RetrieveDataTable(FName Key) const
{
	if (DataTableMaps.Contains(Key))
	{
		return DataTableMaps[Key];
	}

	return nullptr;
}
