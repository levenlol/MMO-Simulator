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

UENUM(BlueprintType)
enum class EMMOSkillCastFailType : uint8
{
	None, // no failure
	WrongTarget,
	Unavailable,
	Cooldown,
	OutOfRange, 
	AlreadyCasting,
	Unspecified // this is still a failure
};

USTRUCT()
struct MMO_SIMULATOR_API FMMODelayAADamage
{
	GENERATED_BODY()
public:
	UPROPERTY()
	AMMOBaseCharacter* Target = nullptr;

	UPROPERTY()
	float TimeToDamage = 0.f;

	UPROPERTY()
	bool bPending = false;

	bool HasPendingDamage() const { return TimeToDamage > 0.f && Target; }

	void DelayDamage(AMMOBaseCharacter* InTarget, float Delay);
	void Clear();
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMMOSkillFailedEvent, UMMOWrapperSkill*, Skill, EMMOSkillCastFailType, FailReason);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMMOSkillStartEvent, UMMOWrapperSkill*, Skill);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MMO_SIMULATOR_API UMMOCombatSystem : public UActorComponent
{
	GENERATED_BODY()
public:
	UMMOCombatSystem();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = Combat)
	bool StartAttack(AMMOBaseCharacter* Target);

	void StopAttack();
	void StopCast();

	UFUNCTION(BlueprintCallable, Category = Combat)
	bool CanAttackTarget(AMMOBaseCharacter* Target) const;

	UFUNCTION(BlueprintPure, Category = Combat)
	AMMOBaseCharacter* GetAutoAttackTarget() const { return AutoAttackTarget; }

	UFUNCTION(BlueprintCallable, Category = Combat)
	void TryCastSkill(AMMOBaseCharacter* Target, const FVector& Location, const int32 Index);

	UFUNCTION(BlueprintCallable, Category = Combat)
	void AbortCastSkill();

	UFUNCTION(BlueprintPure, Category = Combat)
	EMMOSkillCastFailType CanCastSkill(AMMOBaseCharacter* Target, const FVector& Location, const int32 Index) const;

	UFUNCTION(BlueprintCallable, Category = Combat)
	void SetSkills(const TArray<TSubclassOf<UMMOWrapperSkill>>& InSkills);

	UFUNCTION(BlueprintCallable, Category = Combat)
	int32 AddSkill(TSubclassOf<UMMOWrapperSkill> InSkill);

	UFUNCTION(BlueprintPure, Category = Status)
	bool IsAttacking() const;

	UFUNCTION(BlueprintPure, Category = Status)
	bool IsCasting() const;

	UFUNCTION(BlueprintPure, Category = Status)
	bool IsChanneling() const;

	UFUNCTION(BlueprintPure, Category = Status)
	bool IsStunned() const;

	UFUNCTION(BlueprintPure, Category = Status)
	bool CanCharacterAttack() const;

	UFUNCTION(BlueprintPure, Category = Skill)
	float GetRemainingGlobalCooldown() const;

	UFUNCTION(BlueprintPure, Category = Skill)
	float GetRemainingCooldown(int32 SpellIndex) const;

	UFUNCTION(BlueprintPure, Category = Skill)
	bool IsSkillReady(int32 SpellIndex) const;
	
	UFUNCTION(BlueprintPure, Category = Skill)
	float GetCastingPercentage() const;

	UFUNCTION(BlueprintPure, Category = Skill)
	UMMOWrapperSkill* GetCurrentCastingSkill() const;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill)
	float GlobalCooldown = 1.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = Skill, meta = (AllowPrivateAccess))
	TArray<UMMOWrapperSkill*> Skills;

	UPROPERTY(BlueprintAssignable, Category = Skill)
	FMMOSkillFailedEvent OnSkillStartFailed;

private:
	FGameplayTag AttackTag;
	FGameplayTag StunnedTag;
	FGameplayTag CastTag;
	FGameplayTag ChannelingTag;

	UPROPERTY(VisibleAnywhere, Category = Default)
	AMMOBaseCharacter* OwnerCharacter = nullptr;

	UPROPERTY(VisibleAnywhere, Category = Default)
	AMMOBaseCharacter* AutoAttackTarget = nullptr;

	UPROPERTY(VisibleAnywhere, Category = Combat)
	float LastAttackTime = 0.f;

	UPROPERTY(VisibleAnywhere, Category = Spell)
	float LastSpellCastTime = 0.f;

	bool TryAttack(AMMOBaseCharacter* Target);
	
	void DoAutoAttackDamage(AMMOBaseCharacter* Target);
	FMMODelayAADamage DelayAADamage;

	FMMODamage ComputeAutoAttackDamage();

	AMMOBaseWeapon* GetEquippedMainHandWeapon() const;
	AMMOBaseWeapon* GetEquippedOffHandWeapon() const;

	UFUNCTION()
	void OnCharacterChangeWeapon(AMMOBaseCharacter* Sender, AMMOBaseWeapon* New, AMMOBaseWeapon* Old);

	UFUNCTION()
	void OnSkillStartCast(UMMOWrapperSkill* Sender);

	UFUNCTION()
	void OnSkillFinishCast(UMMOWrapperSkill* Sender);

	UFUNCTION()
	void OnSkillStartChanneling(UMMOWrapperSkill* Sender);

	UFUNCTION()
	void OnSkillFinishChanneling(UMMOWrapperSkill* Sender);

	UFUNCTION()
	void OnSkillAbort(UMMOWrapperSkill* Sender);

	UFUNCTION()
	void OnCharacterStunned(AMMOBaseCharacter* Sender);

	UFUNCTION()
	void OnCharacterDeath(AMMOBaseCharacter* Sender);
};