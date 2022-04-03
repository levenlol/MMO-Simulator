// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystem/Skills/MMOInstantiateSkill.h"
#include "Characters/MMOBaseCharacter.h"

#if WITH_EDITORONLY_DATA
void UMMOInstantiateSkill::DrawDebug(const FVector& Location)
{
	if (bDebug)
	{
		if (CollisionShape == EMMOCollisionShape::Box)
			DrawDebugBox(GetWorld(), Location, BoxHalfExtent, FColor::Blue, false, 5.f, 0, 10);
		if (CollisionShape == EMMOCollisionShape::Capsule)
			DrawDebugCapsule(GetWorld(), Location, CapsuleHalfHeight, CapsuleRadius, FQuat::Identity, FColor::Blue, false, 5.f, 0, 10);
		if (CollisionShape == EMMOCollisionShape::Sphere)
			DrawDebugSphere(GetWorld(), Location, Radius, 8, FColor::Blue, false, 5.f, 0, 10);
	}

}
#endif

void UMMOInstantiateSkill::CastAbility(const FMMOSkillInputData& Data)
{
	const FVector Location = Data.TargetActor ? Data.TargetActor->GetActorLocation() : Data.TargetLocation;

#if WITH_EDITORONLY_DATA
	DrawDebug(Location);
#endif

	TArray<FHitResult> Hits;
	FCollisionShape Collision = MakeCollisionShape();
	if (GetWorld()->SweepMultiByChannel(Hits, Location, Location + FVector::UpVector, FQuat::Identity, CollisionChannel, Collision))
	{
		const int32 PossibleTargetsNum = FMath::Min(Hits.Num(), TargetCap);
		CacheTriggeredSkills(PossibleTargetsNum);

		int TargetHit = 0;
		for (int32 i = 0; i < PossibleTargetsNum; i++)
		{
			const FHitResult& Hit = Hits[i];
			AMMOBaseCharacter* Character = Cast<AMMOBaseCharacter>(Hit.GetActor());

			// Cast if: we have a new target, or we consider previous character/owner character.
			if (Character && (Character != OwnerCharacter || bConsiderOwner) && (Character != Data.TargetActor || bConsiderPrevTarget))
			{
				FMMOSkillInputData InputData;
				InputData.TargetLocation = Character->GetActorLocation();
				InputData.TargetActor = Character;

				InputData.SourceLocation = Data.TargetLocation;
				InputData.SourceActor = Data.TargetActor;

				SkillsToPropagate[i]->TryCastAbility(InputData);
				TargetHit++;

				if (TargetHit >= TargetCap)
					break;
			}
		}
	}
}

void UMMOInstantiateSkill::CacheTriggeredSkills(const int32 Num)
{
	// Lazy initialization. also cache and reuse the same skill.
	while (Num > SkillsToPropagate.Num())
	{
		UMMOWrapperSkill* NewSkill = NewObject<UMMOWrapperSkill>(this, SkillsToPropagateClass ? SkillsToPropagateClass : UMMOWrapperSkill::StaticClass());
		NewSkill->Setup(OwnerCharacter);
		NewSkill->CastTime = 0.f; // We do not support cast time on instanced skills. Yet?
		SkillsToPropagate.Add(NewSkill);
	}
}

FCollisionShape UMMOInstantiateSkill::MakeCollisionShape() const
{
	switch (CollisionShape)
	{
	case EMMOCollisionShape::Sphere:
		return FCollisionShape::MakeSphere(Radius);
	case EMMOCollisionShape::Box:
		return FCollisionShape::MakeBox(BoxHalfExtent);
	case EMMOCollisionShape::Capsule:
		return FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);
	default:
		break;
	}

	return FCollisionShape();
}