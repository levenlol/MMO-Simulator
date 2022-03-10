// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "Core/MMOCommon.h"

#include "MMOCharacterGenerator.generated.h"

enum class EMMOCharacterClass : uint8;

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMOGaussianGenerator
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	float Mu = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data, meta = (ClampMin="0.01"))
	float StdDev = 0.f;

	int32 GetRandomValue() const;
};

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FCharacterAttributesGenerator
{
	GENERATED_BODY()
public:
	// Gaussian generator
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	FMMOGaussianGenerator StrengthGenerator;

	// Gaussian generator
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	FMMOGaussianGenerator ConstitutionGenerator;

	// Gaussian generator
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	FMMOGaussianGenerator IntellectGenerator;

	// Gaussian generator
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	FMMOGaussianGenerator DexterityGenerator;

	// Gaussian generator
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	FMMOGaussianGenerator GoldGenerator;

	// Gaussian generator
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	FMMOGaussianGenerator GredinessGenerator;

	// Gaussian generator
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	FMMOGaussianGenerator HappinessGenerator;

	FMMOCharacterAttributes GenerateAttributes() const;
};

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMOCharacterGeneratorDataTable : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	EMMOCharacterClass Class;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	FCharacterAttributesGenerator AttributesGenerator;
};

UCLASS(config = game, defaultconfig)
class MMO_SIMULATOR_API UMMOCharacterGenerator : public UObject
{
	GENERATED_BODY()
public:
	static void Startup(UObject* Outer);
	static void Shutdown();

	UPROPERTY(config)
	FName CharacterDataTableName;

	UPROPERTY(config)
	TArray<FName> Names;

	UFUNCTION(BlueprintPure, Category = Generator)
	FMMOCharacter GenerateCharacter(EMMOCharacterClass InClass, EMMOCharacterRace InRace, int32 Level = 1) const;

	UFUNCTION(BlueprintPure, Category = Generator)
	static UMMOCharacterGenerator* GetCharacterGenerator() { return UMMOCharacterGenerator::Get(); }

	static UMMOCharacterGenerator* Get() { check(Instance); return Instance; }

private:
	static UMMOCharacterGenerator* Instance;

	void Init();
	void Uninit();

	UPROPERTY(EditAnywhere, Category = Data)
	TMap<EMMOCharacterClass, FCharacterAttributesGenerator> AttributesGeneratorMap;
};
