// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystem/MMOWrapperSkill.h"
#include "Characters/MMOBaseCharacter.h"

void UMMOWrapperSkill::Setup(AMMOBaseCharacter* InOwner)
{
	OwnerCharacter = InOwner;

	for (UMMOBaseSkill* Skill : TriggeredSkills)
	{
		Skill->Setup(InOwner);
	}

	LastCastTime = -Cooldown; // Start with no cooldown
	CurrentCastingTime = 0.f; // not casting
	CurrentChannelingTime = 0.f; // not channeling 

	SkillState = EMMOSkillState::Ready;
}

void UMMOWrapperSkill::TryCastAbility(const FMMOSkillInputData& Data)
{
	if (!IsSkillReady())
	{
		UE_LOG(LogTemp, Warning, TEXT("Requested to cast %s ability but failed because it was in status %d."), *GetName(), static_cast<int32>(SkillState));
		return;
	}

	SavedInputData = Data;

	OnSkillStartCast.Broadcast(this);

	if (FMath::IsNearlyZero(CastTime, KINDA_SMALL_NUMBER))
	{
		CastAbility();
	}
	else
	{
		// start cast
		CurrentCastingTime = 0.f;
		SkillState = EMMOSkillState::Casting;
	}
}

void UMMOWrapperSkill::Tick(float DeltaSeconds)
{
	if (SkillState == EMMOSkillState::Casting)
	{
		CurrentCastingTime += DeltaSeconds;

		if (CurrentCastingTime >= CastTime)
		{
			CurrentCastingTime = CastTime;
			CastAbility(); // side effect to change SkillState
		}
	}
}

bool UMMOWrapperSkill::IsInCooldown() const
{
	const float CurrentTime = GetWorld()->GetTimeSeconds();

	return CurrentTime - LastCastTime <= Cooldown;
}

float UMMOWrapperSkill::GetCooldownPercent() const
{
	if (IsInCooldown())
	{
		const float CurrentTime = GetWorld()->GetTimeSeconds();

		return FMath::Clamp((CurrentTime - LastCastTime) / Cooldown, 0.f, 1.f);
	}

	return 1.0f;
}

float UMMOWrapperSkill::GetRemainingCooldown() const
{
	if (IsInCooldown())
	{
		const float CurrentTime = GetWorld()->GetTimeSeconds();

		return FMath::Max(Cooldown - (CurrentTime - LastCastTime), 0.f);
	}

	return 0.0f;
}

float UMMOWrapperSkill::GetCastingPercent() const
{
	if (IsCasting())
	{
		return CurrentCastingTime / CastTime;
	}

	return 0.0f;
}

void UMMOWrapperSkill::CastAbility()
{
	if (ChannelingTime <= 0.f)
	{
		for (UMMOBaseSkill* Skill : TriggeredSkills)
		{
			Skill->CastAbility(SavedInputData);
			Skill->Finish();
		}

		FinishCastAbility();
		OnSkillFinishCast.Broadcast(this);
	}
	else
	{
		// start channeling
		SkillState = EMMOSkillState::Channeling;

		CurrentTick = 0;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UMMOWrapperSkill::TickChanneling, ChannelingTime / static_cast<float>(ChannelingTickNumber), true, 0.f);


		OnSkillFinishCast.Broadcast(this);
		OnSkillStartChanneling.Broadcast(this);
	}
}

void UMMOWrapperSkill::FinishCastAbility()
{
	SkillState = EMMOSkillState::Ready; // put it back in ready status after casting.
	StartCooldown(); // start cooldown
}

void UMMOWrapperSkill::TickChanneling()
{
	for (UMMOBaseSkill* Skill : TriggeredSkills)
	{
		Skill->CastAbility(SavedInputData);
	}

	CurrentTick++;
	if (CurrentTick >= ChannelingTickNumber)
	{
		for (UMMOBaseSkill* Skill : TriggeredSkills)
		{
			Skill->Finish();
		}

		FinishCastAbility();
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);

		OnSkillFinishChanneling.Broadcast(this);
	}
}

void UMMOWrapperSkill::AbortAbility()
{
	if (SkillState != EMMOSkillState::Casting && SkillState != EMMOSkillState::Channeling)
	{
		return;
	}

	for (UMMOBaseSkill* Skill : TriggeredSkills)
	{
		Skill->Abort();
	}

	if (SkillState == EMMOSkillState::Casting)
	{
		// dont cast ability.
		CurrentCastingTime = CastTime;
	}

	if (SkillState == EMMOSkillState::Channeling)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		CurrentChannelingTime = ChannelingTime;
		StartCooldown(); // put it in cooldown
	}

	SkillState = EMMOSkillState::Ready; // back to ready
	OnSkillAborted.Broadcast(this);
}
