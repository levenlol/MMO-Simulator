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
	float RockDisplacementLength = 1600.f;

	UPROPERTY(EditAnywhere, Category = Environment, meta = (ClampMin = "1.0", ClampMax = "20.0"))
	float PillarsDuration = 12.f;

	UPROPERTY(EditAnywhere, Category = Environment)
	TEnumAsByte<ECollisionChannel> TerrainCollisionChannel = ECollisionChannel::ECC_GameTraceChannel6;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

private:
	void SpawnSafetyPillars();

	UPROPERTY()
	TArray<AActor*> SpawnedPillars;

	template<int32 HealthPercent>
	inline bool DeathNova_Condition(AMMOBaseBoss* Boss)
	{
		return DeathNova_Condition(Boss, HealthPercent);
	}

	bool DeathNova_Condition(AMMOBaseBoss* Boss, int32 HealthPercent);
	void DeathNova_Callback(AMMOBaseBoss* Boss);

};
