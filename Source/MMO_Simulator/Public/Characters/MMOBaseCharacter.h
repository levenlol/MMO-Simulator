// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MMOBaseCharacter.generated.h"

UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
	Warrior,
	Paladin,
	Berserker,
	DeathKnight,
	Shaman,
	Monk,
	Priest,
	Druid,
	Mage,
	Warlock
};

UCLASS()
class MMO_SIMULATOR_API AMMOBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMMOBaseCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	ECharacterClass CharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int32 Strength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int32 Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int32 Intellect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int32 Dexterity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int32 Happiness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int32 Greediness;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
