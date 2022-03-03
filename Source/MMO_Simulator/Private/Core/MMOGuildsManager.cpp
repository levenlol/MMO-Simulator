// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MMOGuildsManager.h"
#include "MMOGameInstance.h"

void UMMOGuildsManager::Init()
{
}

void UMMOGuildsManager::Uninit()
{
}

UMMOGuildsManager* UMMOGuildsManager::GetGuildsManager(const UObject* WorldContextObject)
{
    UMMOGameInstance* MMOGameInstance = UMMOGameInstance::GetMMOGameInstance(WorldContextObject);
    if (MMOGameInstance)
    {
        return MMOGameInstance->GetGuildsManager();
    }

    return nullptr;
}
