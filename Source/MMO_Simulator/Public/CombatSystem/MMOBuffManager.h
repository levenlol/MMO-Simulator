// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/MMOCommon.h"

#include "MMOBuffManager.generated.h"

class UMMOBaseBuff;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MMO_SIMULATOR_API UMMOBuffManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMMOBuffManager();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = Debuff)
	FMMODamage ProcessDamageTaken(FMMODamage Damage);

	UFUNCTION(BlueprintCallable, Category = Debuff)
	void AddBuff(UMMOBaseBuff* Buff, float Duration);


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void ProcessBuffs(float DeltaTime);
	void PurgeBuffs();

	UPROPERTY(VisibleInstanceOnly, Category = Buffs)
	TArray<UMMOBaseBuff*> Buffs;
	
	UPROPERTY(VisibleInstanceOnly, Category = Buffs)
	TArray<float> RemainingBuffsTime;
};
