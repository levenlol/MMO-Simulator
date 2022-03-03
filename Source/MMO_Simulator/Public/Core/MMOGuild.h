// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MMOGuild.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class MMO_SIMULATOR_API UMMOGuild : public UObject
{
	GENERATED_BODY()
public:
	static void Startup(UObject* Outer);
	static void Shutdown();

	static UMMOGuild* Get() { check(Instance); return Instance; }

private:
	static UMMOGuild* Instance;

	void Init();
	void Uninit();
};
