// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatSystem/Skills/MMODamageSkill.h"
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
class MMO_SIMULATOR_API UMMOPoolDamageSkill : public UMMOBaseSkill
{
	GENERATED_BODY()
public:
	UMMOPoolDamageSkill();
	
	virtual void Setup(AMMOBaseCharacter* InOwner) override;

	virtual void CastAbility(FMMOSkillInputData Data) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FX)
	TSubclassOf<AMMOPoolActor> PoolClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
	float Duration = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup, meta=(ClampMin="1"))
	int32 TickNumber = 5;

private:
	UPROPERTY()
	AMMOPoolActor* Pool;

	UFUNCTION()
	void Tick();

	FTimerHandle TimerHandle;
	int32 CurrentTick = 0;

	void SetPoolActive(const bool bActive);
};
