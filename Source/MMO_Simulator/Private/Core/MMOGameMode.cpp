// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MMOGameMode.h"
#include "Core/MMOPlayerController.h"


AMMOGameMode::AMMOGameMode() : Super()
{
	// use our custom PlayerController class
	PlayerControllerClass = AMMOPlayerController::StaticClass();
}