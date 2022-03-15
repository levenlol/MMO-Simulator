// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MMOBTTask_AutoAttack.generated.h"

class AMMOBaseCharacter;

struct FMMOBTTask_AutoAttackData
{
	/** time left for attack */
	float DamageDelayTime;
	float AttackSpeed;
	bool bDealtDamage;

	/** target to attack */
	TWeakObjectPtr<AMMOBaseCharacter> TargetWeak;
};

UCLASS()
class MMO_SIMULATOR_API UMMOBTTask_AutoAttack : public UBTTaskNode
{
	GENERATED_BODY()
public:
	
	UMMOBTTask_AutoAttack();
	
	UPROPERTY(Category = Wait, EditAnywhere, meta = (ClampMin = "0.01", UIMin = "0.01", ClampMax = "1.0", UIMax = "1.0"))
	float DamageAtAnimPercentage = 0.5f;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual uint16 GetInstanceMemorySize() const override;
	virtual void DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const override;

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
