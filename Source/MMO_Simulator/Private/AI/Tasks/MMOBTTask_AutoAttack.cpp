// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/MMOBTTask_AutoAttack.h"
#include "AIController.h"
#include "Characters/MMOBaseCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Core/MMOCommon.h"
#include "Weapons/MMOBaseWeapon.h"
#include "CombatSystem/MMOCombatSystem.h"

UMMOBTTask_AutoAttack::UMMOBTTask_AutoAttack()
	: Super()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UMMOBTTask_AutoAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FMMOBTTask_AutoAttackData* MyMemory = CastInstanceNodeMemory<FMMOBTTask_AutoAttackData>(NodeMemory);

	AAIController* AIOwner = OwnerComp.GetAIOwner();
	AMMOBaseCharacter* Character = Cast<AMMOBaseCharacter>(AIOwner->GetPawn());
	UBlackboardComponent* BlackBoard = AIOwner->GetBlackboardComponent();

	if (!IsValid(Character) || !BlackBoard)
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to ExecuteTask: UMMOBTTask_AutoAttack"));
		return EBTNodeResult::Failed;
	}

	AMMOBaseCharacter* Target = Cast<AMMOBaseCharacter>(BlackBoard->GetValueAsObject(FName("TargetActor")));

	if (Character->CombatSystem->CanAttackTarget(Target))
	{
		MyMemory->AttackSpeed = Character->GetMainHandWeapon()->Stats.AttackSpeed;
		MyMemory->DamageDelayTime = MyMemory->AttackSpeed * DamageAtAnimPercentage;
		MyMemory->TargetWeak = Target;
		MyMemory->bDealtDamage = false;

		return EBTNodeResult::InProgress;
	}

	UE_LOG(LogTemp, Error, TEXT("Unable to ExecuteTask: UMMOBTTask_AutoAttack"));
	return EBTNodeResult::Failed;
}

void UMMOBTTask_AutoAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FMMOBTTask_AutoAttackData* MyMemory = CastInstanceNodeMemory<FMMOBTTask_AutoAttackData>(NodeMemory);

	MyMemory->AttackSpeed -= DeltaSeconds;
	MyMemory->DamageDelayTime -= DeltaSeconds;

	AAIController* AIOwner = OwnerComp.GetAIOwner();
	AMMOBaseCharacter* Character = Cast<AMMOBaseCharacter>(AIOwner->GetPawn());

	if (!IsValid(Character) || Character->CombatSystem->IsStunned())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}

	if (!MyMemory->bDealtDamage && MyMemory->DamageDelayTime <= 0.0f)
	{
		// continue execution from this node
		AMMOBaseCharacter* Target = MyMemory->TargetWeak.Get();

		if (!IsValid(Target))
		{
			UE_LOG(LogTemp, Error, TEXT("Unable to Complete Task: UMMOBTTask_AutoAttack"));
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

			return;
		}

		MyMemory->bDealtDamage = true;
		const bool bAttackPerformed = Character->CombatSystem->StartAttack(Target);
	}

	if (MyMemory->AttackSpeed <= 0.f)
	{
		// Should also be synched with animation.
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

EBTNodeResult::Type UMMOBTTask_AutoAttack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FMMOBTTask_AutoAttackData* MyMemory = CastInstanceNodeMemory<FMMOBTTask_AutoAttackData>(NodeMemory);
	MyMemory->DamageDelayTime = 0.f;
	MyMemory->bDealtDamage = true;

	FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);

	return EBTNodeResult::Aborted;
}

void UMMOBTTask_AutoAttack::DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const
{
	Super::DescribeRuntimeValues(OwnerComp, NodeMemory, Verbosity, Values);

	FMMOBTTask_AutoAttackData* MyMemory = CastInstanceNodeMemory<FMMOBTTask_AutoAttackData>(NodeMemory);
	if (MyMemory->DamageDelayTime)
	{
		Values.Add(FString::Printf(TEXT("remaining: %ss"), *FString::SanitizeFloat(MyMemory->DamageDelayTime)));
	}
}

uint16 UMMOBTTask_AutoAttack::GetInstanceMemorySize() const
{
	return sizeof(FMMOBTTask_AutoAttackData);
}