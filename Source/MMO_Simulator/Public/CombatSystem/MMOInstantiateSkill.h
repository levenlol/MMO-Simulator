// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatSystem/MMOBaseSkill.h"
#include "MMOInstantiateSkill.generated.h"

UENUM(BlueprintType)
enum class EMMOCollisionShape : uint8
{
	Sphere,
	Capsule,
	Box
};

/*
	This is basically a container of a Skill (SkillWrapper). 
	For Each target hit we can instantiate a skill.
	This is usefull to avoid the WrapperSkill restriction to have one target.

	IE: if we do a shape collision and hit 6 enemy. we istantiate 6 skill, each with different target.
*/

UCLASS()
class MMO_SIMULATOR_API UMMOInstantiateSkill : public UMMOBaseSkill
{
	GENERATED_BODY()
public:
	virtual void CastAbility(FMMOSkillInputData Data) override;

	UPROPERTY(EditAnywhere, Category = Target)
	TEnumAsByte<ECollisionChannel> CollisionChannel;

	UPROPERTY(EditAnywhere, Category = Target)
	TSubclassOf<UMMOWrapperSkill> SkillsToPropagateClass;

	UPROPERTY(EditAnywhere, Category = Target)
	EMMOCollisionShape CollisionShape = EMMOCollisionShape::Sphere;

	UPROPERTY(EditAnywhere, Category = Target)
	bool bConsiderOwner = true;

	UPROPERTY(EditAnywhere, Category = Target)
	bool bConsiderPrevTarget = false;

	UPROPERTY(EditAnywhere, Category = Target, meta=(ClampMin="1", EditCondition = "CollisionShape == EMMOCollisionShape::Sphere"))
	float Radius = 100.f;

	UPROPERTY(EditAnywhere, Category = Target, meta = (ClampMin = "1", EditCondition = "CollisionShape == EMMOCollisionShape::Box"))
	FVector BoxHalfExtent = FVector::OneVector;

	UPROPERTY(EditAnywhere, Category = Target, meta = (ClampMin = "1", EditCondition = "CollisionShape == EMMOCollisionShape::Capsule"))
	float CapsuleRadius = 1.f;

	UPROPERTY(EditAnywhere, Category = Target, meta = (ClampMin = "1", EditCondition = "CollisionShape == EMMOCollisionShape::Capsule"))
	float CapsuleHalfHeight = 1.f;

	// Maximum number of target
	UPROPERTY(EditAnywhere, Category = Target, meta = (ClampMin = "1", EditCondition = "CollisionShape == EMMOCollisionShape::Sphere"))
	int32 TargetCap = 1000;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = Debug)
	bool bDebug = false;

	void DrawDebug(const FVector& Location);
#endif
private:

	UPROPERTY(VisibleAnywhere, Category = Target)
	TArray<UMMOWrapperSkill*> SkillsToPropagate;

	void CacheTriggeredSkills(const int32 Num);

	FCollisionShape MakeCollisionShape() const;
};
