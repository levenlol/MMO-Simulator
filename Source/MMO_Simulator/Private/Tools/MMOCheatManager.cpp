// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/MMOCheatManager.h"
#include "Core/MMOPlayerController.h"
#include "Characters/MMOBaseHero.h"
#include "Tools/MMOFunctionLibrary.h"

void UMMOCheatManager::Cheat_SetPropertyValue(FString PropertyPath, float Value)
{
    AMMOPlayerController* PC = Cast<AMMOPlayerController>(GetPlayerController());
    if (PC)
    {
        for (AMMOBaseHero* Hero : PC->GetSelectedHeroes())
        {
            UMMOFunctionLibrary::SetPropertyValue(Hero, PropertyPath, Value);
        }
    }
}
