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

	UPROPERTY(EditAnywhere, Category = Environment, meta=(ClampMin="0.0"))
	int32 NumberOfPillars = 8;

	UPROPERTY(EditAnywhere, Category = Environment)
	TSubclassOf<AActor> PillarsClass;

	UPROPERTY(EditAnywhere, Category = Environment, meta = (ClampMin = "100.0"))
	float RockDisplacementLength = 800.f;

	UPROPERTY(EditAnywhere, Category = Environment)
	TEnumAsByte<ECollisionChannel> TerrainCollisionChannel = ECollisionChannel::ECC_GameTraceChannel6;

	virtual void OnDropHealthEvent_Implementation(const FMMOHealthPercentEventData& HealthPercentData) override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

private:
	void SpawnSafetyPillars();
		
};
