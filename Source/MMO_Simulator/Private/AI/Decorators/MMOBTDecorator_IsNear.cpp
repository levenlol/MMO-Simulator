// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorators/MMOBTDecorator_IsNear.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/MMOBaseCharacter.h"
#include "AIController.h"

UMMOBTDecorator_IsNear::UMMOBTDecorator_IsNear()
	: Super()
{
	bAllowAbortLowerPri = true;
	bAllowAbortNone = true;
	bAllowAbortChildNodes = true;
	NodeName = "Is Near";

	// accept only actors and vectors
	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UMMOBTDecorator_IsNear, BlackboardKey));
}

bool UMMOBTDecorator_IsNear::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BlackBoardComponent = OwnerComp.GetBlackboardComponent();
	AMMOBaseCharacter* Character = Cast<AMMOBaseCharacter>(OwnerComp.GetAIOwner()->GetPawn());

	if (BlackBoardComponent && BlackboardKey.IsSet() && Character)
	{
		const FVector Value = BlackBoardComponent->GetValueAsVector(BlackboardKey.SelectedKeyName);
		if (Value != FAISystem::InvalidLocation)
		{
			float CharacterRadius, UnusuedHalfHeigth;
			Character->GetSimpleCollisionCylinder(CharacterRadius, UnusuedHalfHeigth);

			const float Radius = AcceptableRadius + CharacterRadius;
			return (Character->GetActorLocation() - Value).SizeSquared2D() < Radius * Radius;
		}
	}

	return Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
}
