// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/MMOBTService_PickNearestTarget.h"
#include "Characters/MMOBaseCharacter.h"
#include "AI/MMOAIController.h"
#include "CombatSystem/MMOCombatSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Items/MMOBaseWeapon.h"


UMMOBTService_PickNearestTarget::UMMOBTService_PickNearestTarget()
	: Super()
{
	NodeName = "Pick Nearest Target";
	EnemyCollisionChannel = ECollisionChannel::ECC_GameTraceChannel1; // Enemy

	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UMMOBTService_PickNearestTarget, BlackboardKey), AActor::StaticClass());

	INIT_SERVICE_NODE_NOTIFY_FLAGS();
}

void UMMOBTService_PickNearestTarget::OnSearchStart(FBehaviorTreeSearchData& SearchData)
{
	AMMOAIController* AIController = Cast<AMMOAIController>(SearchData.OwnerComp.GetAIOwner());
	if (!AIController)
		return;

	AMMOBaseCharacter* Character = Cast<AMMOBaseCharacter>(AIController->GetPawn());
	UBlackboardComponent* BlackBoard = AIController->GetBlackboardComponent();

	if (!Character || !BlackBoard)
		return;

	const AMMOBaseCharacter* Target = Cast<AMMOBaseCharacter>(BlackBoard->GetValueAsObject(BlackboardKey.SelectedKeyName));
	if (Target && Target->IsAlive())
		return; // we already have a target.

	AMMOBaseWeapon* MainHandWeapon = Character->GetMainHandWeapon();

	if (!MainHandWeapon)
		return;

	const float Radius = MainHandWeapon->Stats.WeaponRange;
	AMMOBaseCharacter* Enemy = GetNearestEnemy(Character->GetActorLocation(), Radius);

	BlackBoard->SetValueAsObject(BlackboardKey.SelectedKeyName, Enemy);
}

AMMOBaseCharacter* UMMOBTService_PickNearestTarget::GetNearestEnemy(const FVector& Location, const float Radius) const
{
	TArray<FHitResult> HitResults;

	FCollisionQueryParams Params;

	GetWorld()->SweepMultiByChannel(HitResults, Location, Location + FVector::UpVector, FQuat::Identity, EnemyCollisionChannel, FCollisionShape::MakeSphere(Radius), Params);
	
	float MinDistance = BIG_NUMBER;
	AMMOBaseCharacter* NearestEnemy = nullptr;
	for (const FHitResult& Hit : HitResults)
	{
		AMMOBaseCharacter* CurrentEnemy = Cast<AMMOBaseCharacter>(Hit.GetActor());
		const float DistSq = (CurrentEnemy->GetActorLocation() - Location).SizeSquared();
		if (DistSq < MinDistance)
		{
			MinDistance = DistSq;
			NearestEnemy = CurrentEnemy;
		}
	}

	return NearestEnemy;
}
