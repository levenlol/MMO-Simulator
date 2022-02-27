#pragma once
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatSystem/MMOBaseSkill.h"
#include "MMOBeamSkill.generated.h"

class UParticleSystemComponent;

UCLASS()
class MMO_SIMULATOR_API AMMOBeam : public AActor
{
	GENERATED_BODY()
public:
	AMMOBeam();

	UPROPERTY(VisibleAnywhere, Category = FX)
	UParticleSystemComponent* ParticleComponent;

	UFUNCTION(BlueprintCallable, Category = Setup)
	void SetBeamSourceTarget(AActor* InSource, AActor* InTarget);

	UFUNCTION(BlueprintPure, Category = Setup)
	AActor* GetTarget() const { return Target; }

	virtual void Tick(float DeltaSeconds) override;

private:
	UPROPERTY()
	AActor* Target = nullptr;

	UPROPERTY()
	AActor* Source;
};

UCLASS()
class MMO_SIMULATOR_API UMMOBeamSkill : public UMMOBaseSkill
{
	GENERATED_BODY()
public:
	UMMOBeamSkill();

	virtual void Setup(AMMOBaseCharacter* InOwner) override;
	virtual void CastAbility(FMMOSkillInputData Data) override;
	
	UPROPERTY(EditDefaultsOnly, Category = FX)
	TSubclassOf<AMMOBeam> BeamClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FX)
	float Duration = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FX, meta=(ClampMin="1"))
	int32 TickNumber = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FX)
	TEnumAsByte<ECollisionChannel> CollisionChannel;

private:
	UPROPERTY(VisibleInstanceOnly, Category = FX)
	AMMOBeam* Beam;

	void SetBeamActive(bool bActive);

	FTimerHandle TimerHandle;
	int32 CurrentTick = 0;
};
