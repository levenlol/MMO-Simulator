// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Characters/MMOBaseCharacter.h"
#include "Chat/MMOChatManagerComponent.h"
#include "MMOGameState.generated.h"

class AMMOBaseHero;
struct FMMOChatMessageData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMMOCharacterEvent, AMMOBaseCharacter*, Sender);

UCLASS()
class MMO_SIMULATOR_API AMMOGameState : public AGameStateBase
{
	GENERATED_BODY()
public:

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category = Game, meta=(WorldContext=WorldContextObject))
	static AMMOGameState* GetMMOGameState(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = Game)
	UMMOChatManagerComponent* GetChatManager();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Heroes)
	TArray<AMMOBaseHero*> Heroes;

	void NotifyStartAttack(AMMOBaseCharacter* Sender);
	void NotifyDeath(AMMOBaseCharacter* Sender);

	UPROPERTY(BlueprintAssignable, Category = Combat)
	FMMOCharacterEvent OnStartAttack;

	UPROPERTY(BlueprintAssignable, Category = Combat)
	FMMOCharacterEvent OnDeath;
private:
	UPROPERTY(VisibleAnywhere, Category = Chat)
	UMMOChatManagerComponent* ChatManager = nullptr;
};
