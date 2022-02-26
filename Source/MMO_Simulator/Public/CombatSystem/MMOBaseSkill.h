// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MMOBaseSkill.generated.h"

class AMMOBaseCharacter;

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMOSkillInputData
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = Input)
	AActor* TargetActor;

	UPROPERTY(BlueprintReadOnly, Category = Input)
	FVector Location;
};

UCLASS(DefaultToInstanced, EditInlineNew, HideDropdown, Blueprintable, BlueprintType)
class MMO_SIMULATOR_API UMMOBaseSkill : public UObject
{
	GENERATED_BODY()
public:
	virtual void Setup(AMMOBaseCharacter* InOwner);

	virtual void Tick(float DeltaSeconds);
	virtual void CastAbility(FMMOSkillInputData Data) {};

	// Triggered Skills from this.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, Category = Skills)
	TArray<UMMOBaseSkill*> TriggeredSkills;

protected:
	UPROPERTY()
	AMMOBaseCharacter* OwnerCharacter;
};

UCLASS(DefaultToInstanced, EditInlineNew, Blueprintable, BlueprintType)
class MMO_SIMULATOR_API UMMOWrapperSkill : public UMMOBaseSkill
{
	GENERATED_BODY()
public:

	// Only need to set the cooldown of "Root" skill.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill)
	float Cooldown = 5.f;

	virtual void Setup(AMMOBaseCharacter* InOwner) override;
	virtual void CastAbility(FMMOSkillInputData Data) override;

	bool IsInCooldown() const;
private:
	float LastCastTime = 0.f;
};
