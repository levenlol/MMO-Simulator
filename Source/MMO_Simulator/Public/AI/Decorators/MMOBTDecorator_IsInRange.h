// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "MMOBTDecorator_IsInRange.generated.h"

class UBlackboardComponent;

UCLASS()
class MMO_SIMULATOR_API UMMOBTDecorator_IsInRange : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()
public:
	UMMOBTDecorator_IsInRange();

	// Used when character is unarmed.
	UPROPERTY(EditAnywhere, Category = Condition, meta = (ClampMin = "0.0"))
	float MinAcceptableRadius = 50.f;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	virtual FString GetStaticDescription() const override;

#if WITH_EDITOR
	virtual FName GetNodeIconName() const override;
#endif // WITH_EDITOR

private:
	FVector GetEndLocation(UBlackboardComponent* BlackBoard, float& OutRadius) const;
};
