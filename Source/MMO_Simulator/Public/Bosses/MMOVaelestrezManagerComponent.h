// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Bosses/MMOBossManagerComponent.h"
#include "MMOVaelestrezManagerComponent.generated.h"

class UMMOWrapperSkill;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MMO_SIMULATOR_API UMMOVaelestrezManagerComponent : public UMMOBossManagerComponent
{
	GENERATED_BODY()

public:	
	UMMOVaelestrezManagerComponent();

	UPROPERTY(EditDefaultsOnly, Category = Skill)
	TSubclassOf<UMMOWrapperSkill> DeathNovaSkill;

	virtual void OnDropHealthEvent_Implementation(const FMMOHealthPercentEventData& HealthPercentData) override;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
