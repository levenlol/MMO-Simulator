// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatSystem/Skills/MMOTimeTickSkill.h"
#include "MMOPoolDamageSkill.generated.h"

class USphereComponent;

UCLASS()
class MMO_SIMULATOR_API AMMOPoolActor : public AMMOFXActor
{
	GENERATED_BODY()
public:
	AMMOPoolActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = FX)
	USphereComponent* SphereComponent;
};

UCLASS(Blueprintable)
class MMO_SIMULATOR_API UMMOPoolDamageSkill : public UMMOTimeTickSkill
{
	GENERATED_BODY()
public:
	UMMOPoolDamageSkill();
	
	virtual void Setup(AMMOBaseCharacter* InOwner) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FX)
	TSubclassOf<AMMOPoolActor> PoolClass;

private:
	UPROPERTY()
	AMMOPoolActor* Pool;

	virtual void StartTick(const FMMOSkillInputData& Data);
	virtual void EndTick(const FMMOSkillInputData& Data);
	virtual void Step(const FMMOSkillInputData& Data, int32 TickCount);

	void SetPoolActive(const bool bActive);
};
