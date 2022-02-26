// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MMOGameState.generated.h"

class AMMOBaseHero;

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
};
