// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CombatSystem/Skills/MMOBaseSkill.h"
#include "MMOWrapperSkill.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMMOOnSkillStart, UMMOWrapperSkill*, Sender);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMMOOnSkillFinish, UMMOWrapperSkill*, Sender);

// Container for MMOBaseSkills
UCLASS(DefaultToInstanced, EditInlineNew, Blueprintable, BlueprintType)
class MMO_SIMULATOR_API UMMOWrapperSkill : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill)
	FName AbilityName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill, meta = (ClampMin = "0"))
	float Cooldown = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill)
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill, meta = (ClampMin = "0"))
	float Range = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill, meta=(ClampMin="0"))
	float CastTime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill, meta = (ClampMin = "0"))
	float ChannelingTime = 0.f;

	// make no sense to do less than 2 tick
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill, meta = (ClampMin = "2", EditCondition="ChannelingTime > 0.0"))
	int32 ChannelingTickNumber = 2;

	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = Skill)
	TEnumAsByte<ECollisionChannel> LOSCollisionChannel = ECollisionChannel::ECC_GameTraceChannel6;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill)
	FGameplayTagContainer Tags;

	void Setup(AMMOBaseCharacter* InOwner);
	virtual void TryCastAbility(const FMMOSkillInputData& Data);
	virtual void AbortAbility();

	// todo: implement and broadcast to triggered skills.
	virtual void End() {};

	void Tick(float DeltaSeconds);

	UFUNCTION(BlueprintPure, Category = Skill)
	float GetCooldownPercent() const;

	UFUNCTION(BlueprintPure, Category = Skill)
	float GetRemainingCooldown() const;

	UFUNCTION(BlueprintPure, Category = Skill)
	bool IsInCooldown() const;

	UFUNCTION(BlueprintPure, Category = Skill)
	FORCEINLINE bool IsSkillReady() const { return !IsInCooldown() && SkillState == EMMOSkillState::Ready; }

	UFUNCTION(BlueprintPure, Category = Skill)
	FORCEINLINE bool IsLocked() const { return SkillState == EMMOSkillState::Locked; };

	UFUNCTION(BlueprintPure, Category = Skill)
	FORCEINLINE bool IsCasting() const { return SkillState == EMMOSkillState::Casting; }

	UFUNCTION(BlueprintPure, Category = Skill)
	FORCEINLINE bool IsChannelling() { return SkillState == EMMOSkillState::Channeling; }
	
	UFUNCTION(BlueprintPure, Category = Skill)
	float GetCastingPercent() const;

	UPROPERTY(BlueprintAssignable, Category = Skill)
	FMMOOnSkillStart OnSkillStartCast;
	
	UPROPERTY(BlueprintAssignable, Category = Skill)
	FMMOOnSkillFinish OnSkillFinishCast;

	UPROPERTY(BlueprintAssignable, Category = Skill)
	FMMOOnSkillStart OnSkillStartChanneling;

	UPROPERTY(BlueprintAssignable, Category = Skill)
	FMMOOnSkillFinish OnSkillFinishChanneling;

	UPROPERTY(BlueprintAssignable, Category = Skill)
	FMMOOnSkillFinish OnSkillAborted;

	// Triggered Skills from this.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, Category = Skills)
	TArray<UMMOBaseSkill*> TriggeredSkills;

protected:
	EMMOSkillState SkillState = EMMOSkillState::Ready;

	UPROPERTY()
	AMMOBaseCharacter* OwnerCharacter;

	void CastAbility();
	virtual void FinishCastAbility();

	float LastCastTime = 0.f; // used to track cooldown
	float CurrentCastingTime = 0.f; // used to cast the ability (if cast time > 0)
	float CurrentChannelingTime = 0.f; // used to channeling the ability (if channeling time > 0)

	FMMOSkillInputData SavedInputData; // Copy of InputData params
private:

	FORCEINLINE void StartCooldown() { LastCastTime = GetWorld()->GetTimeSeconds(); }

	FTimerHandle TimerHandle;
	int32 CurrentTick = 0; // Counter for how many tick we already did. (channeling)

	UFUNCTION()
	void TickChanneling();
};
