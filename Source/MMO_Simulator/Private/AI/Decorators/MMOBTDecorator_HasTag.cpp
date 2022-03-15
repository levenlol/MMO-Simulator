// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorators/MMOBTDecorator_HasTag.h"
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
		if (TagCheck == EMMOTagCheckType::HasAny)
		{
			return Character->StatusTags.HasAny(Tags);
		}
		else if (TagCheck == EMMOTagCheckType::HasAll)
		{
			return Character->StatusTags.HasAll(Tags);
		}
		else if (TagCheck == EMMOTagCheckType::HasAllExact)
		{
			return Character->StatusTags.HasAllExact(Tags);
		}
		else if (TagCheck == EMMOTagCheckType::HasAnyExact)
		{
			return Character->StatusTags.HasAnyExact(Tags);
		}
	}

	return false;
}

FString UMMOBTDecorator_HasTag::GetStaticDescription() const
{
	return FString::Printf(TEXT("%s: %s"), *Super::GetStaticDescription(), *Tags.ToStringSimple());
}
