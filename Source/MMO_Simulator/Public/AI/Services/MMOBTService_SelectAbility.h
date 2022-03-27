// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "MMOBTService_SelectAbility.generated.h"

class UBlackboardComponent;
class AMMOBaseCharacter;

namespace MMOAI
{
	enum ESelectAbilityResult : uint8
	{
		Failed,
		Succeed
	};
}

UCLASS()
class MMO_SIMULATOR_API UMMOBTService_SelectAbility : public UBTService
{
	GENERATED_BODY()
public:
	UMMOBTService_SelectAbility();
	
	UPROPERTY(EditAnywhere, Category = Spell)
	FBlackboardKeySelector SpellSelector;

	UPROPERTY(EditAnywhere, Category = Spell)
	FBlackboardKeySelector SpellLocationSelector;

	UPROPERTY(EditAnywhere, Category = Spell)
	FBlackboardKeySelector SpellTargetSelector;

	UPROPERTY(EditAnywhere, Category = Spell)
	TEnumAsByte<ECollisionChannel> EnemySpellCollisionChannel;

	UPROPERTY(EditAnywhere, Category = Spell)
	TEnumAsByte<ECollisionChannel> FriendlySpellCollisionChannel;

	virtual void OnSearchStart(FBehaviorTreeSearchData& SearchData) override;
private:
	MMOAI::ESelectAbilityResult HandleTargetSpell(AMMOBaseCharacter* Character, UBlackboardComponent* BlackBoard, int32 SpellIndex);
	MMOAI::ESelectAbilityResult HandleSelfCastSpell(AMMOBaseCharacter* Character, UBlackboardComponent* BlackBoard, int32 SpellIndex);

	TArray<FHitResult> GetHitsResults(const FVector& Location, ECollisionChannel CollisionChannel, const float Radius, const AActor* ActorToIgnore = nullptr) const;
};
