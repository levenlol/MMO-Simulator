// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Characters/MMOBaseHero.h"
#include "MMOGameMode.generated.h"

enum class EMMOCharacterClass : uint8;
struct FMMOCharacter;

UCLASS()
class MMO_SIMULATOR_API AMMOGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	AMMOGameMode();

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = Hero)
	TMap<EMMOCharacterClass, TSubclassOf<AMMOBaseHero>> ClassesArchetype;

private:
	void SpawnCharacter(const FMMOCharacter& Character, const FVector& Location);
};
