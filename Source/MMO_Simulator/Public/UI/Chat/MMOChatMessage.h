#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MMOChatMessage.generated.h"

class UTextBlock;
class AMMOBaseCharacter;

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
