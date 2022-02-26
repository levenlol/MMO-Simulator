// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Core/MMOCommon.h"

#include "MMOCombatSystem.generated.h"

class AMMOBaseHero;
class AMMOBaseCharacter;
class UMMOWrapperSkill;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MMO_SIMULATOR_API UMMOCombatSystem : public UActorComponent
{
	GENERATED_BODY()
public:
	UMMOCombatSystem();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = Combat)
	bool StartAttack(AMMOBaseCharacter* Target);

	UFUNCTION(BlueprintCallable, Category = Combat)
	bool CanAttackTarget(AMMOBaseCharacter* Target) const;

	UFUNCTION(BlueprintCallable, Category = Combat)
	void TryCastSkill(AActor* Target, const FVector& Location, const int32 Index);


	UFUNCTION(BlueprintPure, Category = Stats)
	bool IsAttacking() const;

	UFUNCTION(BlueprintPure, Category = Stats)
	bool IsStunned() const;

	UFUNCTION(BlueprintPure, Category = Stats)
	bool CanCharacterAttack() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, instanced, Category = Skill, meta = (AllowPrivateAccess))
	TArray<UMMOWrapperSkill*> Skills;

private:
	UPROPERTY(EditAnywhere, Category = Status, meta=(AllowPrivateAccess))
	FGameplayTag AttackTag;

	UPROPERTY(EditAnywhere, Category = Status, meta = (AllowPrivateAccess))
	FGameplayTag StunnedTag;

	UPROPERTY(VisibleAnywhere, Category = Default)
	AMMOBaseCharacter* OwnerCharacter = nullptr;

	UPROPERTY(VisibleAnywhere, Category = Combat)
	float LastAttackTime = 0.f;

	bool TryAttack(AMMOBaseCharacter* Target);
	void StopAttack();

	FMMODamage ComputeAutoAttackDamage();

	AMMOBaseWeapon* GetEquippedMainHandWeapon() const;
	AMMOBaseWeapon* GetEquippedOffHandWeapon() const;

	UFUNCTION()
	void OnCharacterChangeWeapon(AMMOBaseCharacter* Sender, AMMOBaseWeapon* New, AMMOBaseWeapon* Old);
};