// Copyright Epic Games, Inc. All Rights Reserved.

#include "MMO_SimulatorGameMode.h"
#include "MMO_SimulatorPlayerController.h"
#include "MMO_SimulatorCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMMO_SimulatorGameMode::AMMO_SimulatorGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AMMO_SimulatorPlayerController::StaticClass();
}