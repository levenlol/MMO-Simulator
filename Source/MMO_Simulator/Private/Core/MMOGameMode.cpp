// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MMOGameMode.h"
#include "Core/MMOPlayerController.h"
#include "Characters/MMOBaseHero.h"
#include "Characters/MMOBaseCharacter.h"
#include "MMOGameInstance.h"
#include "Core/MMOCommon.h"
#include "Kismet/GameplayStatics.h"
#include "AI/MMOFormationManager.h"
#include "EngineUtils.h"

AMMOGameMode::AMMOGameMode() : Super()
{
	// use our custom PlayerController class
	PlayerControllerClass = AMMOPlayerController::StaticClass();
}

void AMMOGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Call init on placed on level characters
	for (TActorIterator<AMMOBaseCharacter> CharacterIt(GetWorld(), AMMOBaseCharacter::StaticClass()); CharacterIt; ++CharacterIt)
	{
		AMMOBaseCharacter* Character = *CharacterIt;
		Character->InitializeCharacter(Character->CharacterInfo);
	}

	// Spawn selected heroes
	if (UMMOGameInstance* MMOGameInstance = UMMOGameInstance::GetMMOGameInstance(this))
	{
		AMMOPlayerController* PC = Cast<AMMOPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
		const TArray<FVector> Formation = PC->FormationManager->ComputeSimpleFormation(MMOGameInstance->GetRaidCharacters().Num(), FVector::ZeroVector, FVector(100.f, 0.f, 0.f));

		for (int32 i = 0; i < MMOGameInstance->GetRaidCharacters().Num(); i++)
		{
			const FMMOCharacter& Character = MMOGameInstance->GetRaidCharacters()[i];
			SpawnCharacter(Character, Formation[i]);
		}
	}
}

void AMMOGameMode::SpawnCharacter(const FMMOCharacter& Character, const FVector& Location)
{
	if (ClassesArchetype.Contains(Character.CharacterClass) && ClassesArchetype[Character.CharacterClass])
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		AMMOBaseHero* Hero = GetWorld()->SpawnActor<AMMOBaseHero>(ClassesArchetype[Character.CharacterClass], Location, FRotator::ZeroRotator, SpawnParams);
		Hero->InitializeCharacter(Character);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn RaidCharacter: %s"), *Character.Name.ToString());
	}
}
