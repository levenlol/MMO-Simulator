// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "MMOCheatManager.generated.h"

/**
 * 
 */
UCLASS()
class MMO_SIMULATOR_API UMMOCheatManager : public UCheatManager
{
	GENERATED_BODY()
public:
	UFUNCTION(exec)
	void Cheat_SetPropertyValue(FString PropertyPath, float Value);

	UFUNCTION(exec)
	void Cheat_StunHeroes(float Seconds);

	UFUNCTION(exec)
	void Cheat_PostMessage_Random(FString Message);
};
