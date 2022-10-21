// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"
#include "MMOBaseSkill.generated.h"

class AMMOBaseCharacter;
class UParticleSystemComponent;
class UMMOWrapperSkill;

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
	FGameplayTag TauntTag;
	FGameplayTag EnemyTag;
	FGameplayTag FriendlyTag;
	FGameplayTag Buff;
	FGameplayTag Debuff;

	FORCEINLINE static const FMMOSkillTags& Get() { return SkillTags; }

protected:
	virtual void AddTags() override
	{
		UGameplayTagsManager& Manager = UGameplayTagsManager::Get();

		LocationTag = Manager.AddNativeGameplayTag(FName("Skill.Location"));
		SelfCastLocationTag = Manager.AddNativeGameplayTag(FName("Skill.Location.Self"));
		TargetCastLocationTag = Manager.AddNativeGameplayTag(FName("Skill.Location.Target"));
		TargetTag = Manager.AddNativeGameplayTag(FName("Skill.Actor.Target"));
		TauntTag = Manager.AddNativeGameplayTag(FName("Skill.Taunt"));
		SelfTargetTag = Manager.AddNativeGameplayTag(FName("Skill.Actor.Self"));
		FriendlyTag = Manager.AddNativeGameplayTag(FName("Skill.Friendly"));
		EnemyTag = Manager.AddNativeGameplayTag(FName("Skill.Enemy"));
		Buff = Manager.AddNativeGameplayTag(FName("Skill.Buff"));
		Debuff = Manager.AddNativeGameplayTag(FName("Skill.Debuff"));
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

	virtual void CastAbility(const FMMOSkillInputData& Data) {}
	virtual void AbortAbility() {}
	virtual void Finish() {}
	virtual void Abort() {}

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

