// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MMOChat.generated.h"

class UMMOChatMessage;
class UVerticalBox;

UCLASS()
class MMO_SIMULATOR_API UMMOChat : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(EditAnywhere, Category = Chat)
	int32 MessageHistorySize = 5;

	UPROPERTY(EditAnywhere, Category = Chat)
	TSubclassOf<UMMOChatMessage> ChatMessageClass;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* MessagesRoot;

	UFUNCTION()
	void OnCharacterTalk(const FMMOChatMessageData& MessageData);
private:
	void DisplayChatMessages();

	UPROPERTY(VisibleAnywhere, Category = Chat)
	TArray<FMMOChatMessageData> Messages;

	UPROPERTY(VisibleAnywhere, Category = Chat)
	TArray<UMMOChatMessage*> ChatMessages;
};
