// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structures/HttpGPTChatTypes.h"
#include "MMOChatManagerComponent.generated.h"

class AMMOBaseCharacter;
class UHttpGPTChatRequest;

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMOChatMessageData
{
	GENERATED_BODY()
public:
	FMMOChatMessageData() = default;
	FMMOChatMessageData(const TWeakObjectPtr<AMMOBaseCharacter>& InOwner, const FString& InMessage);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Message")
	FString OwnerName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Message")
	FString Message;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Message")
	FLinearColor NameColor;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Message")
	TWeakObjectPtr<AMMOBaseCharacter> OwnerCharacter = nullptr;

	bool IsEmpty() const { return OwnerName.Len() == 0 && Message.Len() == 0; }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMMOChatEvent, const FMMOChatMessageData&, MessageData);

UCLASS(Config=game, Blueprintable, BlueprintType, ClassGroup=(Chat), meta=(BlueprintSpawnableComponent))
class MMO_SIMULATOR_API UMMOChatManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMMOChatManagerComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(config)
	bool bEnabled = false;

	// Sent as 'System' message to ChatGPT
	UPROPERTY(EditAnywhere, Category = Chat)
	FString ChatManagerInstruction = "";

	// [TODO] Send last 'MessageHistorySize' chat message to chat gpt. Used to give context.
	UPROPERTY(EditAnywhere, Category = Chat)
	int32 MessageHistorySize = 5;

	UFUNCTION(BlueprintCallable, Category = Chat)
	void WriteMessage(AMMOBaseCharacter* Sender, const FString& Message);

	UFUNCTION(BlueprintPure, Category = Chat, meta = (WorldContext = WorldContextObject))
	static UMMOChatManagerComponent* GetChatManagerComponent(const UObject* WorldContextObject);

	UPROPERTY(BlueprintAssignable, Category = Chat)
	FMMOChatEvent OnTalking;

	UFUNCTION(BlueprintPure, Category = Chat)
	const TArray<FMMOChatMessageData>& GetChatHistory() const { return ChatHistory; }

	UFUNCTION(BlueprintPure, Category = Chat)
	bool IsTalkingToPlayer() const { return bEnabled && bTalkToPlayer; }

protected:
	UPROPERTY(EditAnywhere, Category = Chat)
	bool bTalkToPlayer = true;

	virtual void BeginPlay() override;
private:
	void TalkToPlayer();

	TArray<FHttpGPTChatMessage> CollectChatMessages() const;

	UFUNCTION()
	void OnResponceReceived(const FHttpGPTChatResponse& Response);

	UPROPERTY(VisibleAnywhere, Category = Chat)
	TArray<FMMOChatMessageData> ChatHistory;

	UPROPERTY()
	TArray<UHttpGPTChatRequest*> Requests;
};
