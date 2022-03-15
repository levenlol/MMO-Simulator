// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/MMOBTService_SelectAbility.h"

UMMOBTService_SelectAbility::UMMOBTService_SelectAbility()
	: Super()
{
	NodeName = "Select Ability";

	INIT_SERVICE_NODE_NOTIFY_FLAGS();
}

void UMMOBTService_SelectAbility::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
}
