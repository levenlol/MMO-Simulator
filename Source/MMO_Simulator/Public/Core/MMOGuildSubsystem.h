// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MMOGuildSubsystem.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, Abstract)
class MMO_SIMULATOR_API UMMOGuildSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    // Begin USubsystem
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    // End USubsystem

private:
    // All my variables
};