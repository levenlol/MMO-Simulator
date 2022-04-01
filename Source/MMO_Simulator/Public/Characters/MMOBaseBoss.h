// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/MMOBaseEnemy.h"
#include "MMOBaseBoss.generated.h"

/**
 * 
 */
UCLASS()
class MMO_SIMULATOR_API AMMOBaseBoss : public AMMOBaseEnemy
{
	GENERATED_BODY()
public:
	virtual void Stun_Implementation(float InDuration) override;
};
