// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"
#include "MMOBaseSkill.generated.h"

class AMMOBaseCharacter;

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
	FGameplayTag TargetTag;
	FGameplayTag EnemyTag;
	FGameplayTag FriendlyTag;
	FGameplayTag Buff;

	FORCEINLINE static const FMMOSkillTags& Get() { return SkillTags; }

protected:
	virtual void AddTags() override
	{
		UGameplayTagsManager& Manager = UGameplayTagsManager::Get();

		LocationTag = Manager.AddNativeGameplayTag(FName("Skill.Location"));
		TargetTag = Manager.AddNativeGameplayTag(FName("Skill.Target"));
		FriendlyTag = Manager.AddNativeGameplayTag(FName("Skill.Friendly"));
		EnemyTag = Manager.AddNativeGameplayTag(FName("Skill.Enemy"));
		Buff = Manager.AddNativeGameplayTag(FName("Skill.Buff"));
	}
private:
	static FMMOSkillTags SkillTags;
};

UCLASS(DefaultToInstanced, EditInlineNew, HideDropdown, Blueprintable, BlueprintType)
class MMO_SIMULATOR_API UMMOBaseSkill : public UObject
{
	GENERATED_BODY()
public:
	virtual void Setup(AMMOBaseCharacter* InOwner);

	virtual void CastAbility(FMMOSkillInputData Data) {};

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

UCLASS(DefaultToInstanced, EditInlineNew, Blueprintable, BlueprintType)
class MMO_SIMULATOR_API UMMOWrapperSkill : public UMMOBaseSkill
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill)
	FGameplayTagContainer Tags;

	virtual void Setup(AMMOBaseCharacter* InOwner) override;
	virtual void CastAbility(FMMOSkillInputData Data) override;

	void Tick(float DeltaSeconds);

	UFUNCTION(BlueprintPure, Category = Skill)
	bool IsInCooldown() const;

	UFUNCTION(BlueprintPure, Category = Skill)
	float GetCooldownPercent() const;
	
	UFUNCTION(BlueprintPure, Category = Skill)
	bool IsLocked() const { return bLocked; };

	UFUNCTION(BlueprintPure, Category = Skill)
	bool IsCasting() const { return bCasting; }
	
	UFUNCTION(BlueprintPure, Category = Skill)
	float GetCastingPercent() const;
private:
	float LastCastTime = 0.f; // used to track cooldown
	float CurrentCastingTime = 0.f; // used to cast the ability (if cast time > 0)

	bool bLocked = false;
	bool bCasting = false;

	void FinishCastAbility();
	FMMOSkillInputData SavedInputData; // Copy of InputData params
};
