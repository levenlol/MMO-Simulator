// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MMOBTTask_WaitAttackDelay.generated.h"

struct FMMOWaitAttackDelayMemory
{
	/** time left */
	float RemainingWaitTime;
};

/**
 * Wait task node.
 * Wait for the specified time when executed.
 */
UCLASS()
class MMO_SIMULATOR_API UMMOBTTask_WaitAttackDelay : public UBTTaskNode
{
	GENERATED_BODY()
public:

	UMMOBTTask_WaitAttackDelay(const FObjectInitializer& ObjectInitializer);

	/** wait time in seconds */
	UPROPERTY(Category = Wait, EditAnywhere, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float WaitTime;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual uint16 GetInstanceMemorySize() const override;
	virtual void DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const override;

#if WITH_EDITOR
	virtual FName GetNodeIconName() const override;
#endif // WITH_EDITOR

protected:

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
