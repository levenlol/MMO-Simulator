// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/Object.h"
#include "CoreMinimal.h"
#include "Core/MMOCommon.h"
#include "MMOBaseDebuff.generated.h"

class AMMOBaseCharacter;

// Init and Uninit are called one time when Debuff is created.
// Activate/Deactivate is called from DebuffManager.
// Tick is called from DebuffManager
UCLASS(Abstract, Blueprintable, BlueprintType)
class MMO_SIMULATOR_API UMMOBaseDebuff : public UObject
{
	GENERATED_BODY()
public:
	UMMOBaseDebuff();

	virtual void Init(AMMOBaseCharacter* InOwner);
	virtual void Activate(AMMOBaseCharacter* InTarget);
	virtual void ReceiveExecute();
	virtual void Deactivate();
	virtual void Uninit();

	void Tick(float DeltaTime);

	UPROPERTY(EditAnywhere, Category = Setup)
	float TickTime = 1.f;
protected:
	UPROPERTY()
	AMMOBaseCharacter* OwnerCharacter;

	UPROPERTY()
	AMMOBaseCharacter* TargetCharacter;
private:
	float AccDeltaTime = 0.f;
};
