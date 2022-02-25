// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MMOBTDecorator_IsInRange.h"
#include "Characters/MMOBaseCharacter.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

UMMOBTDecorator_IsInRange::UMMOBTDecorator_IsInRange()
	: Super()
{
	NodeName = "Is In Range";
}

bool UMMOBTDecorator_IsInRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AMMOBaseCharacter* BaseCharacter = Cast<AMMOBaseCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();

	if (!BaseCharacter || !BlackBoard)
	{
		return false;
	}

	float AcceptableRadius = MinAcceptableRadius;

	if (BaseCharacter->HasEquippedAnyWeapons())
	{
		AcceptableRadius = BaseCharacter->GetWeaponRange();
	}

	AActor* Target = Cast<AActor>(BlackBoard->GetValueAsObject(GetSelectedBlackboardKey()));
	if (Target)
	{
		const FVector Location = BaseCharacter->GetActorLocation();
		const FVector EndLocation = Target->GetActorLocation();

		return (EndLocation - Location).SizeSquared() <= AcceptableRadius * AcceptableRadius;
	}

	return false;
}

FString UMMOBTDecorator_IsInRange::GetStaticDescription() const
{
	FString KeyDesc("invalid");
	if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass() ||
		BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		KeyDesc = BlackboardKey.SelectedKeyName.ToString();
	}

	return FString::Printf(TEXT("%s: %s"), *Super::GetStaticDescription(), *KeyDesc);
}

#if WITH_EDITOR

FName UMMOBTDecorator_IsInRange::GetNodeIconName() const
{
	return FName("BTEditor.Graph.BTNode.Decorator.ReachedMoveGoal.Icon");
}

#endif	// WITH_EDITOR
