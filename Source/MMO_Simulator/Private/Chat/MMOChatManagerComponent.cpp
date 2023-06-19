// Fill out your copyright notice in the Description page of Project Settings.


#include "Chat/MMOChatManagerComponent.h"
#include "Core/MMOGameState.h"
#include "Tasks/HttpGPTChatRequest.h"
#include "Characters/MMOBaseHero.h"
#include "Utils/MMOGameplayUtils.h"

#if !UE_BUILD_SHIPPING
static FAutoConsoleCommandWithWorldAndArgs CVarChatGPT_Enabled(
	TEXT("gpt.Enabled"),
	TEXT("Arguments: 0/1\n")
	TEXT("Controls whether the chat manager use chat gpt for message generation."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() == 0)
				return;

			const bool bShouldBeEnabled = Args[0].ToBool();
			UMMOChatManagerComponent* ChatManagerComponent = UMMOChatManagerComponent::GetChatManagerComponent(World);

			if (ChatManagerComponent)
			{
				ChatManagerComponent->bEnabled = bShouldBeEnabled;
				UE_LOG(LogTemp, Log, TEXT("Chat GPT for Game Chat - %s"), bShouldBeEnabled ? TEXT("Enabled") : TEXT("Disabled"));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Cannot find ChatManagerComponent"));
			}
		}),
	ECVF_Cheat);
#endif

UMMOChatManagerComponent::UMMOChatManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UMMOChatManagerComponent::WriteMessage(AMMOBaseCharacter* Sender, const FString& Message)
{
	if (!Sender || Message.IsEmpty())
		return;

	if (AMMOGameState* MMOGameState = AMMOGameState::GetMMOGameState(this))
	{
		FMMOChatMessageData MessageData(Sender, Message);
		ChatHistory.Add(MessageData);

		AMMOBaseHero* Hero = Cast<AMMOBaseHero>(Sender);
		if (IsTalkingToPlayer() && Hero && Hero->GuildRole == EMMOGuildRole::GuildMaster)
		{
			TalkToPlayer();
		}

		OnTalking.Broadcast(MessageData);
	}
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
	Request->ProcessCompleted.AddDynamic(this, &UMMOChatManagerComponent::OnResponceReceived);
	Request->Activate();

	Requests.Add(Request);
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

void UMMOChatManagerComponent::OnResponceReceived(const FHttpGPTChatResponse& Response)
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
	WriteMessage(UMMOGameplayUtils::GetRandomGuildHero(this, true), ReceivedMessage);
}


void UMMOChatManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

