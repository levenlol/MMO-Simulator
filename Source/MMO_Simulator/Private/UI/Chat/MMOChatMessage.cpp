#include "UI/Chat/MMOChatMessage.h"
#include "Components/TextBlock.h"
#include "Characters/MMOBaseCharacter.h"
#include "Data/MMODataFinder.h"

FMMOChatMessageData::FMMOChatMessageData(const TWeakObjectPtr<AMMOBaseCharacter>& InOwner, const FString& InMessage)
	: OwnerName(InOwner.IsValid() ? InOwner->CharacterInfo.Name.ToString() : FString())
	, Message(InMessage)
	, OwnerCharacter(InOwner)
{
	NameColor = InOwner.IsValid() 
		? UMMODataFinder::Get()->GetColorFromClass(InOwner->CharacterInfo.CharacterClass)
		: FLinearColor::White;
}

void UMMOChatMessage::SetMessage(const FMMOChatMessageData& InMessageData)
{
	MessageData = InMessageData;
	if (MessageData.IsEmpty())
	{
		NameTextBlock->SetText(FText::GetEmpty());
		MessageTextBlock->SetText(FText::GetEmpty());
	}
	else
	{
		NameTextBlock->SetColorAndOpacity(FSlateColor(InMessageData.NameColor));
		NameTextBlock->SetText(FText::FromString(MessageData.OwnerName + TEXT(": ")));
		MessageTextBlock->SetText(FText::FromString(MessageData.Message));
	}
}
