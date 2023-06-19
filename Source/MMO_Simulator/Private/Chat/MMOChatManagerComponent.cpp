// Fill out your copyright notice in the Description page of Project Settings.


#include "Chat/MMOChatManagerComponent.h"
#include "Core/MMOGameState.h"
#include "Tasks/HttpGPTChatRequest.h"
#include "Characters/MMOBaseHero.h"
#include "Utils/MMOGameplayUtils.h"

UMMOChatManagerComponent::UMMOChatManagerComponent()
{
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bCanEverTick = true;
}

void UMMOChatManagerComponent::WriteMessage(AMMOBaseCharacter* Sender, const FString& Message)
{
	if (!Sender || Message.IsEmpty())
		return;

	if (AMMOGameState* MMOGameState = AMMOGameState::GetMMOGameState(this))
	{
		FMMOChatMessageData MessageData(Sender, Message);
		WriteMessage(MessageData);
	}
}

void UMMOChatManagerComponent::WriteMessage(const FMMOChatMessageData& MessageData)
{
	if (MessageData.IsEmpty())
		return;

	ChatHistory.Add(MessageData);

	AMMOBaseHero* Hero = Cast<AMMOBaseHero>(MessageData.OwnerCharacter.Get());
	if (IsTalkingToPlayer() && Hero && Hero->GuildRole == EMMOGuildRole::GuildMaster)
	{
		TalkToPlayer();
	}

	OnTalking.Broadcast(MessageData);
}

UMMOChatManagerComponent* UMMOChatManagerComponent::GetChatManagerComponent(const UObject* WorldContextObject)
{
	if (AMMOGameState* GameState = AMMOGameState::GetMMOGameState(WorldContextObject))
	{
		return GameState->GetChatManager();
	}

	return nullptr;
}

void UMMOChatManagerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMMOChatManagerComponent::TalkToPlayer()
{
	TArray<FHttpGPTChatMessage> HttpMessages = CollectChatMessages();
	UHttpGPTChatRequest* Request = UHttpGPTChatRequest::SendMessages_DefaultOptions(this, HttpMessages);
	Request->ProcessCompleted.AddDynamic(this, &UMMOChatManagerComponent::OnResponseSuccess);
	Request->ErrorReceived.AddDynamic(this, &UMMOChatManagerComponent::OnResponseError);

	Request->Activate();
}

TArray<FHttpGPTChatMessage> UMMOChatManagerComponent::CollectChatMessages() const
{
	TArray<FHttpGPTChatMessage> Messages;

	if (!ChatManagerInstruction.IsEmpty())
	{
		Messages.Add(FHttpGPTChatMessage(
			EHttpGPTChatRole::System,
			ChatManagerInstruction
		));
	}

	// [TODO] Add History.

	// Add last message sent. This is the actual answer we need to get an answer.
	Messages.Add(FHttpGPTChatMessage(
		EHttpGPTChatRole::User,
		ChatHistory.Last().Message
	));

	return Messages;
}

void UMMOChatManagerComponent::OnResponseError(const FHttpGPTChatResponse& Response)
{
	if (!Response.bSuccess)
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to generate response. Code: %s, Type: %s, Message: %s"),
			*Response.Error.Code.ToString(),
			*Response.Error.Type.ToString(),
			*Response.Error.Message);
	}
}

TArray<FMMOChatMessageData> UMMOChatManagerComponent::TryParseCharacterResponses(const FString& RawResponse)
{
	TArray<FMMOChatMessageData> CharacterResponses;
	
	TArray<FString> Lines;
	RawResponse.ParseIntoArrayLines(Lines);

	for (const FString& Line : Lines)
	{
		const int32 Index = Line.Find(TEXT(":"));

		if (Index >= 0)
		{
			const FString HeroName = Line.LeftChop(Line.Len() - Index);
			const FString Message = Line.RightChop(Index + 1);

			AMMOBaseHero* Hero = UMMOGameplayUtils::GetHeroByName(this, *HeroName);

			if (!Hero || Hero->GuildRole == EMMOGuildRole::GuildMaster)
				Hero = UMMOGameplayUtils::GetRandomGuildHero(this, true);

			CharacterResponses.Add(FMMOChatMessageData(
				Hero,
				Message
			));
		}
		else
		{
			AMMOBaseHero* Hero = UMMOGameplayUtils::GetRandomGuildHero(this, true);
			CharacterResponses.Add(FMMOChatMessageData(
				Hero,
				Line
			));
		}
	}
	
	return CharacterResponses;
}

void UMMOChatManagerComponent::OnResponseSuccess(const FHttpGPTChatResponse& Response)
{
	if (!Response.bSuccess)
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to generate response. Code: %s, Type: %s, Message: %s"),
			*Response.Error.Code.ToString(),
			*Response.Error.Type.ToString(),
			*Response.Error.Message);
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Generated Response Chat-GPT"));
	UE_LOG(LogTemp, Log, TEXT("Usage - Promp Tokens: %d, Completion Tokens: %d, Total Tokens: %d"),
		Response.Usage.PromptTokens,
		Response.Usage.CompletionTokens,
		Response.Usage.TotalTokens);

	const FString& ReceivedMessage = Response.Choices[0].Message.Content;
	PendingMessages = TryParseCharacterResponses(ReceivedMessage);

	UE_LOG(LogTemp, Log, TEXT("Message Received: %s"), *ReceivedMessage);
}


void UMMOChatManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PostPendingMessageDelay > 0.f)
	{
		PostPendingMessageDelay -= DeltaTime;
		return;
	}

	if (PendingMessages.Num() == 0)
		return;

	FMMOChatMessageData PendingMessage = PendingMessages.Pop();
	WriteMessage(PendingMessage);

	PostPendingMessageDelay = 3.f;
}

