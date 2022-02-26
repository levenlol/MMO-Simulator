// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MMOGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/MMOBaseHero.h"
#include "EngineUtils.h"

void AMMOGameState::BeginPlay()
{
	Super::BeginPlay();

	for (TActorIterator<AMMOBaseHero> It(GetWorld(), AMMOBaseHero::StaticClass()); It; ++It)
	{
		AMMOBaseHero* Hero = *It;
		Heroes.Add(Hero);
	}
}

AMMOGameState* AMMOGameState::GetMMOGameState(const UObject* WorldContextObject)
{
	return Cast<AMMOGameState>(UGameplayStatics::GetGameState(WorldContextObject));
}
