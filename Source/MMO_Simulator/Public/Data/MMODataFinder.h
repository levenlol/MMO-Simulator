// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MMOGameInstance.h"
#include "MMODataFinder.generated.h"

UCLASS(config = game, defaultconfig)
class MMO_SIMULATOR_API UMMODataFinder : public UObject
{
	GENERATED_BODY()
public:
	static void Startup(UObject* Outer);
	static void Shutdown();

	UPROPERTY(config)
	FName AnimationDataTableName;

	static UMMODataFinder* Get() { check(Instance); return Instance; }

	UAnimSequenceBase* GetAnimSequence(const FMMOWeaponTypeCouple& WeaponCouple) const;

private:
	static UMMODataFinder* Instance;

	UPROPERTY(VisibleAnywhere, Category = Animation)
	TMap<FMMOWeaponTypeCouple, UAnimSequenceBase*> AnimationsMap;

	void Init();
	void Uninit();

	void ParseAnimationDataTable();
};
