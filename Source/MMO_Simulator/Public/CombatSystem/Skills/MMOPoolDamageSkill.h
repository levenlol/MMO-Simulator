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

UCLASS()
class MMO_SIMULATOR_API UMMOPoolDamageSkill : public UMMOTimeTickSkill
{
	GENERATED_BODY()
public:
	UMMOPoolDamageSkill();
	
	virtual void Setup(AMMOBaseCharacter* InOwner) override;

	virtual void CastAbility(FMMOSkillInputData Data) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FX)
	TSubclassOf<AMMOPoolActor> PoolClass;

private:
	UPROPERTY()
	AMMOPoolActor* Pool;

	virtual void Step(int32 TickCount) override;
	virtual void StartTick() override;
	virtual void EndTick() override;

	void SetPoolActive(const bool bActive);
};
