// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chat/MMOChat.h"
#include "Core/MMOGameState.h"
#include "UI/Chat/MMOChatMessage.h"
#include "Components/VerticalBox.h"


void UMMOChat::NativeConstruct()
{
	Super::NativeConstruct();

	AMMOGameState* GameState = AMMOGameState::GetMMOGameState(this);
	if (!GameState)
		return;

	// that's not the best thing, cuz if we send a message and the chat still didnt construct we will miss the message.
	// but for now is Ok-ish.

	GameState->OnTalking.AddDynamic(this, &UMMOChat::OnCharacterTalk);

	// remove all children
	MessagesRoot->ClearChildren();

	ChatMessages.Empty();
	ChatMessages.SetNum(MessageHistorySize);

	// little trick to display the last message first, and clip the top ones.
	MessagesRoot->RenderTransform.Angle = 180.f;

	for (UMMOChatMessage*& ChatMessage : ChatMessages)
	{
		ChatMessage = CreateWidget<UMMOChatMessage>(this, ChatMessageClass);
		ChatMessage->RenderTransform.Angle = 180.f; // little trick
		MessagesRoot->AddChildToVerticalBox(ChatMessage);
	}

	DisplayChatMessages();
}

void UMMOChat::NativeDestruct()
{
	Super::NativeDestruct();

	AMMOGameState* GameState = AMMOGameState::GetMMOGameState(this);
	if (!GameState)
		return;

	GameState->OnTalking.RemoveDynamic(this, &UMMOChat::OnCharacterTalk);
}

void UMMOChat::OnCharacterTalk(const FMMOChatMessageData& MessageData)
{
	if (Messages.Num() > MessageHistorySize)
		Messages.RemoveAt(0);
	
	Messages.Add(MessageData);

	DisplayChatMessages();
}

void UMMOChat::DisplayChatMessages()
{
	int32 i = 0;

	// First write in chat the written message in reverse order
	for (; i < Messages.Num(); i++)
	{
		const FMMOChatMessageData& MessageData = Messages[i];
		ChatMessages[Messages.Num() - i - 1]->SetMessage(MessageData);
	}

	for (; i < MessageHistorySize; i++)
	{
		ChatMessages[i]->SetMessage(FMMOChatMessageData());
	}
}
