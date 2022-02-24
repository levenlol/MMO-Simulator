// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/MMOBaseCharacter.h"
#include "MMOBaseEnemy.generated.h"

class UMMOAggroManagerComponent;
class USphereComponent;

UCLASS()
class MMO_SIMULATOR_API AMMOBaseEnemy : public AMMOBaseCharacter
{
	GENERATED_BODY()
public:
	AMMOBaseEnemy();

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Aggro)
	UMMOAggroManagerComponent* AggroManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Aggro)
	USphereComponent* AggroSphere;

protected:
	virtual void BeginPlay() override;
};
