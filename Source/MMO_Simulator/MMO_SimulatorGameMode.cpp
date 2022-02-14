// Copyright Epic Games, Inc. All Rights Reserved.

#include "MMO_SimulatorGameMode.h"
#include "MMO_SimulatorPlayerController.h"
#include "MMO_SimulatorCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMMO_SimulatorGameMode::AMMO_SimulatorGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AMMO_SimulatorPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}