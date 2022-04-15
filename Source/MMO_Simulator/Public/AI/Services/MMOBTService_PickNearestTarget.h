// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "MMOBTService_PickNearestTarget.generated.h"

class AMMOBaseCharacter;
enum ECollisionChannel : uint8;

UCLASS()
class MMO_SIMULATOR_API UMMOBTService_PickNearestTarget : public UBTService_BlackboardBase
{
	GENERATED_BODY()
public:
	UMMOBTService_PickNearestTarget();

	UPROPERTY(EditAnywhere, Category = Spell)
	TEnumAsByte<ECollisionChannel> EnemyCollisionChannel;

	virtual void OnSearchStart(FBehaviorTreeSearchData& SearchData) override;
private:
	AMMOBaseCharacter* GetNearestEnemy(const FVector& Location, const float Radius) const;
};
