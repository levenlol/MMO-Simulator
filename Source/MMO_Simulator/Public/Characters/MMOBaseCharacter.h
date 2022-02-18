// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Core/MMOCommon.h"
#include "MMOBaseCharacter.generated.h"

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
	int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	FCharacterStats Stats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int32 Gold;

	UFUNCTION(BlueprintPure, Category = Stats)
	FORCEINLINE float GetHealthPercent() const { return static_cast<float>(Stats.Health) / static_cast<float>(Stats.MaxHealth); }

	UFUNCTION(BlueprintPure, Category = Stats)
	FORCEINLINE float GetResourcePercent() const { return static_cast<float>(Stats.Resources) / static_cast<float>(Stats.MaxResources); }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
