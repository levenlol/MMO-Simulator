// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MMOCombatSystem.generated.h"

class AMMOBaseHero;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MMO_SIMULATOR_API UMMOCombatSystem : public UActorComponent
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};