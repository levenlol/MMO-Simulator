// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/MMOBaseCharacter.h"
#include "MMOBaseEnemy.generated.h"

/**
 * 
 */
UCLASS()
class MMO_SIMULATOR_API AMMOBaseEnemy : public AMMOBaseCharacter
{
	GENERATED_BODY()
public:
	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;
};
