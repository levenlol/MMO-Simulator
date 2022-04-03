// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"
#include "MMOBaseSkill.generated.h"

class AMMOBaseCharacter;
class UParticleSystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMMOOnSkillStart, UMMOWrapperSkill*, Sender);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMMOOnSkillFinish, UMMOWrapperSkill*, Sender);

UENUM()
enum class EMMOSkillState : uint8
{
	Ready,
	Casting,
	Channeling,
	Locked
};

UCLASS()
class MMO_SIMULATOR_API AMMOFXActor : public AActor
{
	GENERATED_BODY()
public:
	AMMOFXActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = FX)
	UParticleSystemComponent* ParticleComponent;
};

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMOSkillInputData
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = Input)
	AMMOBaseCharacter* TargetActor;

	UPROPERTY(BlueprintReadOnly, Category = Input)
	FVector TargetLocation;

	UPROPERTY(BlueprintReadOnly, Category = Input)
	AMMOBaseCharacter* SourceActor;

	UPROPERTY(BlueprintReadOnly, Category = Input)
	FVector SourceLocation;
};

struct MMO_SIMULATOR_API FMMOSkillTags : public FGameplayTagNativeAdder
{
	FGameplayTag LocationTag;
	FGameplayTag SelfCastLocationTag;
	FGameplayTag TargetCastLocationTag;
	FGameplayTag TargetTag;
	FGameplayTag SelfTargetTag; // if spell is castable on self
	FGameplayTag EnemyTag;
	FGameplayTag FriendlyTag;
	FGameplayTag Buff;

	FORCEINLINE static const FMMOSkillTags& Get() { return SkillTags; }

protected:
	virtual void AddTags() override
	{
		UGameplayTagsManager& Manager = UGameplayTagsManager::Get();

		LocationTag = Manager.AddNativeGameplayTag(FName("Skill.Location"));
		SelfCastLocationTag = Manager.AddNativeGameplayTag(FName("Skill.Location.Self"));
		TargetCastLocationTag = Manager.AddNativeGameplayTag(FName("Skill.Location.Target"));
		TargetTag = Manager.AddNativeGameplayTag(FName("Skill.Target"));
		SelfTargetTag = Manager.AddNativeGameplayTag(FName("Skill.Target.Self"));
		FriendlyTag = Manager.AddNativeGameplayTag(FName("Skill.Friendly"));
		EnemyTag = Manager.AddNativeGameplayTag(FName("Skill.Enemy"));
		Buff = Manager.AddNativeGameplayTag(FName("Skill.Buff"));
	}
private:
	static FMMOSkillTags SkillTags;
};

// Base class for skill functionality
UCLASS(DefaultToInstanced, EditInlineNew, HideDropdown, Blueprintable, BlueprintType)
class MMO_SIMULATOR_API UMMOBaseSkill : public UObject
{
	GENERATED_BODY()
public:
	virtual void Setup(AMMOBaseCharacter* InOwner);

	virtual void CastAbility(const FMMOSkillInputData& Data) {};
	virtual void AbortAbility() {};
	virtual void Finish() {};
	virtual void Abort() {};

	// Triggered Skills from this.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, Category = Skills)
	TArray<UMMOBaseSkill*> TriggeredSkills;

protected:
	UPROPERTY()
	AMMOBaseCharacter* OwnerCharacter;

	const UMMOWrapperSkill* GetOuterSkill() const;
private:
	const UMMOBaseSkill* GetOuterSkill_Rec(const UMMOBaseSkill* InSkill) const;
};

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill)
	FGameplayTagContainer Tags;

	void Setup(AMMOBaseCharacter* InOwner);
	void CastAbility(const FMMOSkillInputData& Data);
	void AbortAbility();

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

private:
	float LastCastTime = 0.f; // used to track cooldown
	float CurrentCastingTime = 0.f; // used to cast the ability (if cast time > 0)
	float CurrentChannelingTime = 0.f; // used to channeling the ability (if channeling time > 0)

	EMMOSkillState SkillState = EMMOSkillState::Ready;

	void FinishCastAbility();
	FORCEINLINE void StartCooldown() { LastCastTime = GetWorld()->GetTimeSeconds(); }

	FTimerHandle TimerHandle;
	int32 CurrentTick = 0; // Counter for how many tick we already did. (channeling)

	UFUNCTION()
	void TickChanneling();

	UPROPERTY()
	AMMOBaseCharacter* OwnerCharacter;

	FMMOSkillInputData SavedInputData; // Copy of InputData params
};
