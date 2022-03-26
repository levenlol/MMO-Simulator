// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatSystem/Skills/MMOBaseSkill.h"
#include "MMOProjectileSkill.generated.h"

class USphereComponent;

UCLASS()
class MMO_SIMULATOR_API AMMOProjectile : public AMMOFXActor
{
	GENERATED_BODY()
public:
	AMMOProjectile();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = FX)
	USphereComponent* SphereComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setup)
	float Speed = 200.f;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = Setup)
	void SetTarget(AActor* InTarget) { Target = InTarget; }

	UFUNCTION(BlueprintPure, Category = Setup)
	AActor* GetTarget() const { return Target; }

private:
	UPROPERTY()
	AActor* Target = nullptr;
};

UCLASS()
class MMO_SIMULATOR_API UMMOProjectileSkill : public UMMOBaseSkill
{
	GENERATED_BODY()
public:
	virtual void Setup(AMMOBaseCharacter* InOwner) override;

	virtual void CastAbility(FMMOSkillInputData Data) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	TSubclassOf<AMMOProjectile> ProjectileClass;
private:
	UPROPERTY()
	AMMOProjectile* Projectile;

	UFUNCTION()
	void OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void SetProjectileActive(bool bActive);
};
