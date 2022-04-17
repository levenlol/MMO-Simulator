#include "AI/Services/MMOBTService_AutoAttack.h"

#include "AIController.h"
#include "Characters/MMOBaseCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CombatSystem/MMOCombatSystem.h"

void UMMOBTService_AutoAttack::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* Controller = OwnerComp.GetAIOwner();
	AMMOBaseCharacter* Character = Controller->GetPawn<AMMOBaseCharacter>();
	UBlackboardComponent* BlackboardComponent = Controller->GetBlackboardComponent();

	if (!BlackboardComponent || !Character)
	{
		return;
	}

	AMMOBaseCharacter* TargetActor = Cast<AMMOBaseCharacter>(BlackboardComponent->GetValueAsObject(TEXT("TargetActor")));

	if (TargetActor == nullptr)
	{
		Character->CombatSystem->StopAttack();
	}
	else
	{
		Character->CombatSystem->StartAttack(TargetActor);
	}
}