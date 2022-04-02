// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/MMOBTService_SelectAbility.h"
#include "Characters/MMOBaseCharacter.h"
#include "AI/MMOAIController.h"
#include "CombatSystem/MMOCombatSystem.h"
#include "CombatSystem/Skills/MMOBaseSkill.h"
#include "BehaviorTree/BlackboardComponent.h"

#define CHECK_RETURN_SPELLHANDLED(foo) MMOAI::ESelectAbilityResult Result = foo;\
	if(Result == MMOAI::ESelectAbilityResult::Succeed) return;

UMMOBTService_SelectAbility::UMMOBTService_SelectAbility()
	: Super()
{
	NodeName = "Select Ability";
	EnemySpellCollisionChannel = ECollisionChannel::ECC_GameTraceChannel1; // Enemy
	FriendlySpellCollisionChannel = ECollisionChannel::ECC_GameTraceChannel3; // Hero
	INIT_SERVICE_NODE_NOTIFY_FLAGS();
}

void UMMOBTService_SelectAbility::OnSearchStart(FBehaviorTreeSearchData& SearchData)
{
	AMMOAIController* AIController = Cast<AMMOAIController>(SearchData.OwnerComp.GetAIOwner());
	if (!AIController)
		return;

	AMMOBaseCharacter* Character = Cast<AMMOBaseCharacter>(AIController->GetPawn());
	UBlackboardComponent* BlackBoard = AIController->GetBlackboardComponent();

	if (!Character || !BlackBoard)
		return;

	const int32 SelectedSpell = BlackBoard->GetValueAsInt(SpellSelector.SelectedKeyName) - 1;
	if (Character->CombatSystem->Skills.IsValidIndex(SelectedSpell))
	{
		// we already have a spell to cast.
		return;
	}

	for (int32 i = 0; i < Character->CombatSystem->Skills.Num(); i++)
	{
		UMMOWrapperSkill* WrapperSkill = Character->CombatSystem->Skills[i];
		if (AIController->IsSpellHandledByThePlayer(i))
			continue;

		// TODO: better pick spell logic.
		const FMMOSkillTags& SkillTags = FMMOSkillTags::Get();
		if (WrapperSkill->Tags.HasTag(SkillTags.TargetTag))
		{
			CHECK_RETURN_SPELLHANDLED(HandleTargetSpell(Character, BlackBoard, i));
		}
		else if (WrapperSkill->Tags.HasTag(SkillTags.SelfCastLocationTag))
		{
			CHECK_RETURN_SPELLHANDLED(HandleSelfCastSpell(Character, BlackBoard, i));
		}
	}
}

MMOAI::ESelectAbilityResult UMMOBTService_SelectAbility::HandleTargetSpell(AMMOBaseCharacter* Character, UBlackboardComponent* BlackBoard, int32 SpellIndex)
{
	const FMMOSkillTags& SkillTags = FMMOSkillTags::Get();

	UMMOWrapperSkill* Skill = Character->CombatSystem->Skills[SpellIndex];	
	AMMOBaseCharacter* TargetCharacter = nullptr;

	TArray<FHitResult> HitResults;
	if (Skill->Tags.HasTag(SkillTags.EnemyTag))
	{
		HitResults = GetHitsResults(Character->GetActorLocation(), EnemySpellCollisionChannel, Skill->Range);
		TargetCharacter = RetrieveBestEnemyTarget(HitResults);
	}
	else if (Skill->Tags.HasTag(SkillTags.FriendlyTag))
	{
		const bool bSelfCastable = Skill->Tags.HasTag(SkillTags.SelfTargetTag); // true if the spell can be cast on self.
		HitResults = GetHitsResults(Character->GetActorLocation(), FriendlySpellCollisionChannel, Skill->Range, bSelfCastable ? nullptr : Character);
		TargetCharacter = RetrieveBestFriendlyTarget(HitResults);
	}
	else ensure(0 && "Didnt support that kind of spell yet");

	if (TargetCharacter == nullptr)
		return MMOAI::ESelectAbilityResult::Failed;


	if (Character->CombatSystem->CanCastSkill(TargetCharacter, TargetCharacter->GetActorLocation(), SpellIndex) == EMMOSkillCastFailType::None)
	{
		BlackBoard->SetValueAsInt(SpellSelector.SelectedKeyName, SpellIndex + 1); // spells are 1 based
		BlackBoard->SetValueAsVector(SpellLocationSelector.SelectedKeyName, TargetCharacter->GetActorLocation());
		BlackBoard->SetValueAsObject(SpellTargetSelector.SelectedKeyName, TargetCharacter);

		return MMOAI::ESelectAbilityResult::Succeed;
	}

	return MMOAI::ESelectAbilityResult::Failed;
}

MMOAI::ESelectAbilityResult UMMOBTService_SelectAbility::HandleSelfCastSpell(AMMOBaseCharacter* Character, UBlackboardComponent* BlackBoard, int32 SpellIndex)
{
	if (Character->CombatSystem->CanCastSkill(Character, Character->GetActorLocation(), SpellIndex) == EMMOSkillCastFailType::None)
	{
		BlackBoard->SetValueAsInt(SpellSelector.SelectedKeyName, SpellIndex + 1); // spells are 1 based
		BlackBoard->SetValueAsVector(SpellLocationSelector.SelectedKeyName, Character->GetActorLocation());
		BlackBoard->SetValueAsObject(SpellTargetSelector.SelectedKeyName, Character);
		
		return MMOAI::ESelectAbilityResult::Succeed;
	}

	return MMOAI::ESelectAbilityResult::Failed;
}

TArray<FHitResult> UMMOBTService_SelectAbility::GetHitsResults(const FVector& Location, ECollisionChannel CollisionChannel, const float Radius, const AActor* ActorToIgnore) const
{
	TArray<FHitResult> HitResults;
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(ActorToIgnore);
	GetWorld()->SweepMultiByChannel(HitResults, Location, Location + FVector::UpVector, FQuat::Identity, CollisionChannel, FCollisionShape::MakeSphere(Radius), Params);

	return HitResults;
}

AMMOBaseCharacter* UMMOBTService_SelectAbility::RetrieveBestEnemyTarget(const TArray<FHitResult>& HitResults) const
{
	if (HitResults.Num() > 0)
	{
		// todo better select. consider player selection
		return Cast<AMMOBaseCharacter>(HitResults[0].GetActor());
	}

	return nullptr;
}

AMMOBaseCharacter* UMMOBTService_SelectAbility::RetrieveBestFriendlyTarget(const TArray<FHitResult>& HitResults) const
{
	// todo consider player selection
	float MinHealthPercent = 1.f;
	AMMOBaseCharacter* Character = nullptr;

	for (int32 i = 0; i < HitResults.Num(); i++)
	{
		AMMOBaseCharacter* CurrentChar = Cast<AMMOBaseCharacter>(HitResults[i].GetActor());
		if (CurrentChar && CurrentChar->GetHealthPercent() <= MinHealthPercent)
		{
			MinHealthPercent = CurrentChar->GetHealthPercent();
			Character = CurrentChar;
		}
	}

	return Character;
}

#undef CHECK_RETURN_SPELLHANDLED