// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MMOGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/MMOBaseHero.h"
#include "EngineUtils.h"
#include "UI/Chat/MMOChatMessage.h"

void AMMOGameState::BeginPlay()
{
	Super::BeginPlay();

	for (TActorIterator<AMMOBaseHero> It(GetWorld(), AMMOBaseHero::StaticClass()); It; ++It)
	{
		AMMOBaseHero* Hero = *It;
		Heroes.Add(Hero);
	}
}

AMMOGameState* AMMOGameState::GetMMOGameState(const UObject* WorldContextObject)
{
	return Cast<AMMOGameState>(UGameplayStatics::GetGameState(WorldContextObject));
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

void AMMOGameState::NotifyChat(AMMOBaseCharacter* Sender, const FString& Message)
{
	if (Sender && Message.Len() >= 0)
	{
		FMMOChatMessageData MessageData(Sender, Message);
		OnTalking.Broadcast(MessageData);
	}
}
