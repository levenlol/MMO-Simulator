#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Chat/MMOChatManagerComponent.h"
#include "MMOChatMessage.generated.h"

class UTextBlock;
class AMMOBaseCharacter;

UCLASS()
class MMO_SIMULATOR_API UMMOChatMessage : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* NameTextBlock = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* MessageTextBlock = nullptr;

	UFUNCTION(BlueprintCallable, Category = Message)
	void SetMessage(const FMMOChatMessageData& InMessageData);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Message)
	FMMOChatMessageData MessageData;
};
