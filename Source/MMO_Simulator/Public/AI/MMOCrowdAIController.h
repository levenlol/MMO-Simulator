// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/MMOAIController.h"
#include "MMOCrowdAIController.generated.h"

/**
 * 
 */
UCLASS()
class MMO_SIMULATOR_API AMMOCrowdAIController : public AMMOAIController
{
	GENERATED_BODY()
public:
	AMMOCrowdAIController(const FObjectInitializer& ObjInit);
	
};
