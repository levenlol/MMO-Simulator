// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MMODebuffManager.generated.h"

class UMMOBaseDebuff;
class AMMOBaseCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MMO_SIMULATOR_API UMMODebuffManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMMODebuffManager();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = Debuff)
	void AddDebuff(UMMOBaseDebuff* Debuff, float Duration);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = Debuff)
	TArray<UMMOBaseDebuff*> Debuffs;

	UPROPERTY(VisibleAnywhere, Category = Debuff)
	TArray<float> DebuffsRemainingDuration;

	void ProcessActiveDebuffs(float DeltaTime);
	void PurgeElapsedDebuffs();

	AMMOBaseCharacter* GetOwnerCharacter() const { return Cast<AMMOBaseCharacter>(GetOwner()); }
};
