// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Guilds/MMOGuild.h"
#include "MMOGuildsManager.generated.h"


UCLASS(Blueprintable, Abstract)
class MMO_SIMULATOR_API UMMOGuildsManager : public UObject
{
    GENERATED_BODY()
public:
    void Init();
    void Uninit();

    UFUNCTION(BlueprintPure, meta = (WorldContext = WorldContextObject))
    static UMMOGuildsManager* GetGuildsManager(const UObject* WorldContextObject);

    UPROPERTY(VisibleAnywhere, Category = Guilds)
    TMap<FName, FMMOGuild> GuildMap;
};