// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "Core/MMOCommon.h"
#include "MMODataFinder.generated.h"

class UAnimSequenceBase;

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMOAnimationDataTable : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	FMMOWeaponTypeCouple WeaponsType;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimSequenceBase* WeaponAnimation;
};

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
