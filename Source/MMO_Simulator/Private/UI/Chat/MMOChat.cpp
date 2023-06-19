// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chat/MMOChat.h"
#include "Chat/MMOChatManagerComponent.h"
#include "UI/Chat/MMOChatMessage.h"
#include "Components/VerticalBox.h"
#include "Components/EditableTextBox.h"
#include "Characters/MMOBaseHero.h"
#include "Utils/MMOGameplayUtils.h"

void UMMOChat::NativeConstruct()
{
	Super::NativeConstruct();

	ChatManager = UMMOChatManagerComponent::GetChatManagerComponent(this);
	if (!ChatManager)
		return;

	// Hide chat
	ChatTextBox->SetVisibility(ESlateVisibility::Collapsed);
	ChatTextBox->SetIsReadOnly(true);

	ChatTextBox->OnTextCommitted.AddDynamic(this, &UMMOChat::OnCommitText);

	ChatManager->OnTalking.AddDynamic(this, &UMMOChat::OnCharacterTalk);

	// Init chat history
	const TArray<FMMOChatMessageData>& History = ChatManager->GetChatHistory();

	const int32 SafeHistorySize = FMath::Min(History.Num(), MessageHistorySize);
	for (int32 i = 0; i < SafeHistorySize; i++)
	{
		Messages.Add(History[History.Num() - i - 1]);
	}

	// remove all children
	MessagesRoot->ClearChildren();

	ChatMessages.Empty();
	ChatMessages.SetNum(MessageHistorySize);

	// little trick to display the last message first, and clip the top ones.
	FWidgetTransform TrickRenderTransform = MessagesRoot->GetRenderTransform();
	MessagesRoot->SetRenderTransform(TrickRenderTransform);

	TrickRenderTransform.Angle = 180.f;

	for (UMMOChatMessage*& ChatMessage : ChatMessages)
	{
		ChatMessage = CreateWidget<UMMOChatMessage>(this, ChatMessageClass);
		ChatMessage->SetRenderTransform(TrickRenderTransform); // little trick
		MessagesRoot->AddChildToVerticalBox(ChatMessage);
	}

	DisplayChatMessages();
}

void UMMOChat::NativeDestruct()
{
	Super::NativeDestruct();

	ChatTextBox->OnTextCommitted.RemoveDynamic(this, &UMMOChat::OnCommitText);

	if (!ChatManager)
		return;

	ChatManager->OnTalking.RemoveDynamic(this, &UMMOChat::OnCharacterTalk);
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
	const int32 SafeMessageNum = FMath::Min(MessageHistorySize, Messages.Num());
	for (; i < SafeMessageNum; i++)
	{
		const FMMOChatMessageData& MessageData = Messages[i];
		ChatMessages[Messages.Num() - i - 1]->SetMessage(MessageData);
	}

	for (; i < MessageHistorySize; i++)
	{
		ChatMessages[i]->SetMessage(FMMOChatMessageData());
	}
}

void UMMOChat::OnCommitText(const FText& Text, ETextCommit::Type CommitMethod)
{
	ChatTextBox->SetVisibility(ESlateVisibility::Hidden);
	ChatTextBox->SetIsReadOnly(true);
	ChatTextBox->SetText(FText::GetEmpty());

	if (!ChatManager)
		return;

	ChatManager->WriteMessage(UMMOGameplayUtils::GetHeroesOfRole(this, EMMOGuildRole::GuildMaster)[0], Text.ToString());
}
