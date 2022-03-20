// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MMOEquipGenerator.generated.h"

UCLASS(config = game, defaultconfig)
class MMO_SIMULATOR_API UMMOEquipGenerator : public UObject
{
	GENERATED_BODY()
public:
	static void Startup(UObject* Outer);
	static void Shutdown();

private:
	static UMMOEquipGenerator* Instance;

	void Init();
	void Uninit();
};
