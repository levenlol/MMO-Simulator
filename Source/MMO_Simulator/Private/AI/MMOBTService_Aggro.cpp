// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MMOBTService_Aggro.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Characters/MMOBaseEnemy.h"
#include "AI/MMOAggroManagerComponent.h"


UMMOBTService_Aggro::UMMOBTService_Aggro()
{
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UMMOBTService_Aggro, BlackboardKey), AActor::StaticClass());
	NodeName = "AggroService";

	bNotifyTick = true;
	bNotifyOnSearch = true;
	bTickIntervals = true;
	bCallTickOnSearchStart = false;
	bRestartTimerOnEachActivation = false;
}

void UMMOBTService_Aggro::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	AAIController* AIController = OwnerComp.GetAIOwner();

	if (!BlackBoard || !AIController)
	{
		UE_LOG(LogBehaviorTree, Error, TEXT("UMMOBTService_Aggro cannot find blackboard or aicontroller."));
		return;
	}

	if (AMMOBaseEnemy* Enemy = Cast<AMMOBaseEnemy>(AIController->GetPawn()))
	{
		if (AMMOBaseCharacter* MostDangerous = Enemy->AggroManager->GetMostDangerousCharacter())
		{
			BlackBoard->SetValueAsObject(GetSelectedBlackboardKey(), MostDangerous);
		}
		else
		{
			BlackBoard->ClearValue(BlackboardKey.GetSelectedKeyID());
		}
	}

}
