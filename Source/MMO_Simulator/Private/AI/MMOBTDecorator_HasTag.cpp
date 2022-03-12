// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MMOBTDecorator_HasTag.h"
#include "AIController.h"
#include "Characters/MMOBaseCharacter.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

UMMOBTDecorator_HasTag::UMMOBTDecorator_HasTag()
	: Super()
{
	NodeName = "Has Gameplay Tag";
}

bool UMMOBTDecorator_HasTag::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AMMOBaseCharacter* Character = Cast<AMMOBaseCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (IsValid(Character))
	{
		return Character->HasTag(Tag);
	}

	return false;
}

FString UMMOBTDecorator_HasTag::GetStaticDescription() const
{
	return FString::Printf(TEXT("%s: %s"), *Super::GetStaticDescription(), *Tag.ToString());
}
