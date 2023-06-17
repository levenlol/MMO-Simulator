// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MMOGameState.generated.h"

class AMMOBaseCharacter;
class AMMOBaseHero;
struct FMMOChatMessageData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMMOCharacterEvent, AMMOBaseCharacter*, Sender);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMMOChatEvent, const FMMOChatMessageData&, MessageData);

UCLASS()
class MMO_SIMULATOR_API AMMOGameState : public AGameStateBase
{
	GENERATED_BODY()
public:

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category = Game, meta=(WorldContext=WorldContextObject))
	static AMMOGameState* GetMMOGameState(const UObject* WorldContextObject);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Heroes)
	TArray<AMMOBaseHero*> Heroes;

	void NotifyStartAttack(AMMOBaseCharacter* Sender);
	void NotifyDeath(AMMOBaseCharacter* Sender);

	void NotifyChat(AMMOBaseCharacter* Sender, const FString& Message);

	UPROPERTY(BlueprintAssignable, Category = Combat)
	FMMOCharacterEvent OnStartAttack;

	UPROPERTY(BlueprintAssignable, Category = Combat)
	FMMOCharacterEvent OnDeath;

	UPROPERTY(BlueprintAssignable, Category = Chat)
	FMMOChatEvent OnTalking;
};
