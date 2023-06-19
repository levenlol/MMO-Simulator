// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/MMOCheatManager.h"
#include "Core/MMOPlayerController.h"
#include "Characters/MMOBaseHero.h"
#include "Tools/MMOFunctionLibrary.h"
#include "Utils/MMOGameplayUtils.h"
#include "Chat/MMOChatManagerComponent.h"

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

void UMMOCheatManager::Cheat_StunHeroes(float Seconds)
{
    AMMOPlayerController* PC = Cast<AMMOPlayerController>(GetPlayerController());
    if (PC)
    {
        for (AMMOBaseHero* Hero : PC->GetSelectedHeroes())
        {
            Hero->Stun(Seconds);
        }
    }
}

void UMMOCheatManager::Cheat_PostMessage_Random(FString Message)
{
    TArray<AMMOBaseHero*> Heroes = UMMOGameplayUtils::GetHeroes(this);
    if (Heroes.Num() == 0)
        return;
    
    const int32 Index = FMath::RandRange(0, Heroes.Num() - 1);
    UMMOChatManagerComponent* ChatManager = UMMOChatManagerComponent::GetChatManagerComponent(this);

    if (ChatManager)
    {
        ChatManager->WriteMessage(Heroes[Index], Message);
    }
}
