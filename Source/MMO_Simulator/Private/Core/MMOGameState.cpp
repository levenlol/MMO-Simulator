// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MMOGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/MMOBaseHero.h"
#include "EngineUtils.h"
#include "UI/Chat/MMOChatMessage.h"

void AMMOGameState::BeginPlay()
{
	Super::BeginPlay();

	bool bGM = false;
	for (TActorIterator<AMMOBaseHero> It(GetWorld(), AMMOBaseHero::StaticClass()); It; ++It)
	{
		AMMOBaseHero* Hero = *It;
		Heroes.Add(Hero);

		bGM = bGM || Hero->GuildRole == EMMOGuildRole::GuildMaster;
	}

	// Sanity check.
	if (!bGM && Heroes.Num() > 0)
		Heroes[0]->GuildRole = EMMOGuildRole::GuildMaster;
}

AMMOGameState* AMMOGameState::GetMMOGameState(const UObject* WorldContextObject)
{
	return Cast<AMMOGameState>(UGameplayStatics::GetGameState(WorldContextObject));
}

UMMOChatManagerComponent* AMMOGameState::GetChatManager()
{
	if (!ChatManager)
	{
		ChatManager = FindComponentByClass<UMMOChatManagerComponent>();
	}

	return ChatManager;
}

void AMMOGameState::NotifyStartAttack(AMMOBaseCharacter* Sender)
{
	if (!Sender)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot notify start attack."));
		return;
	}

	OnStartAttack.Broadcast(Sender);
}

void AMMOGameState::NotifyDeath(AMMOBaseCharacter* Sender)
{
	if (!Sender)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot notify death."));
		return;
	}

	OnDeath.Broadcast(Sender);
}
