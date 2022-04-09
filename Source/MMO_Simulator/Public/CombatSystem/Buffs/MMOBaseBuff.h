// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/Object.h"
#include "CoreMinimal.h"
#include "MMOBaseBuff.generated.h"

class AMMOBaseCharacter;

UCLASS(Blueprintable, BlueprintType)
class MMO_SIMULATOR_API UMMOBaseBuff : public UObject
{
	GENERATED_BODY()
public:
	UMMOBaseBuff();

	UPROPERTY(EditAnywhere, Category = Setup)
	float TickTime = 1.f;

	virtual void Init(AMMOBaseCharacter* Owner);
	virtual void Activate();
	virtual void ReceiveExecute() {}
	virtual void Deactivate() {}
	virtual void Uninit() {}

	void Tick(float DeltaTime);
protected:
	UPROPERTY()
	AMMOBaseCharacter* OwnerCharacter;

private:

	float AccDeltaTime = 0.f;
};
