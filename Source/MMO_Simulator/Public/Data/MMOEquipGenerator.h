// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Core/MMOCommon.h"

#include "MMOEquipGenerator.generated.h"


UCLASS(config = game, defaultconfig)
class MMO_SIMULATOR_API UMMOEquipGenerator : public UObject
{
	GENERATED_BODY()
public:
	static void Startup(UObject* Outer);
	static void Shutdown();

	UFUNCTION(BlueprintPure, Category = Equip)
	static UMMOEquipGenerator* GetEquipGenerator() { return Get(); };

	static UMMOEquipGenerator* Get() { check(Instance); return Instance; }

	UFUNCTION(BlueprintCallable, Category = Equip)
	EMMORarityType PickRarityFromProbabilityDistribution(TArray<float> ProbabilityDistribution) const;

	UFUNCTION(BlueprintCallable, Category = Equip)
	FMMOItemStats GenerateEquipOfKind(int32 ItemLevel, EMMORarityType Rarity, EMMOArmorSlotType ArmorSlotType, EMMOArmorType ArmorType, const FString& Qualifier = "");

	UFUNCTION(BlueprintCallable, Category = Equip)
	FString GetEquipSlotName(EMMOArmorSlotType ArmorSlotType) const;

private:
	static UMMOEquipGenerator* Instance;

	void Init();
	void Uninit();
};
