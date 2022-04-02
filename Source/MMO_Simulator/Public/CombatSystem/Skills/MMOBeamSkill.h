#pragma once
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatSystem/Skills/MMOTimeTickSkill.h"
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

UCLASS(Blueprintable)
class MMO_SIMULATOR_API UMMOBeamSkill : public UMMOTimeTickSkill
{
	GENERATED_BODY()
public:
	UMMOBeamSkill();

	virtual void Setup(AMMOBaseCharacter* InOwner) override;
	
	UPROPERTY(EditDefaultsOnly, Category = FX)
	TSubclassOf<AMMOBeam> BeamClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Damage)
	TEnumAsByte<ECollisionChannel> CollisionChannel;

	AMMOBeam* GetBeam() const { return Beam; }

private:
	UPROPERTY(VisibleInstanceOnly, Category = FX)
	AMMOBeam* Beam;

	virtual void StartTick(const FMMOSkillInputData& Data);
	virtual void EndTick(const FMMOSkillInputData& Data);
	virtual void Step(const FMMOSkillInputData& Data, int32 TickCount);

	void SetBeamActive(bool bActive);
};
