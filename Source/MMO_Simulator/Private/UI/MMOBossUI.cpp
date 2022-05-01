// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MMOBossUI.h"
#include "Components/Image.h"
#include "Characters/MMOBaseCharacter.h"
#include "Components/TextBlock.h"


UMMOBossUI::UMMOBossUI(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	HealthParameterName = FName("Progress");
	ManaParameterName = FName("Progress");

	CharacterReference = nullptr;
}

void UMMOBossUI::SetHealth(float InPercent)
{
	if (HealthBar)
	{
		InPercent = FMath::Clamp(InPercent, 0.f, 1.f);
		HealthBar->GetDynamicMaterial()->SetScalarParameterValue(HealthParameterName, InPercent);
	}
}

void UMMOBossUI::SetMana(float InPercent)
{
	if (ManaBar)
	{
		InPercent = FMath::Clamp(InPercent, 0.f, 1.f);
		ManaBar->GetDynamicMaterial()->SetScalarParameterValue(HealthParameterName, InPercent);
	}
}

void UMMOBossUI::SetName(const FString& InName)
{
	if (BossNameTextBlock)
	{
		BossNameTextBlock->SetText(FText::FromString(InName));
	}
}

void UMMOBossUI::SetCharacter(AMMOBaseCharacter* InCharacter) 
{ 
	CharacterReference = InCharacter;

	if (CharacterReference)
	{
		SetName(CharacterReference->CharacterInfo.Name.ToString());
	}
}

void UMMOBossUI::NativeConstruct()
{
	Super::NativeConstruct();

	AccumulatedTickTime = 0.f;
}

void UMMOBossUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (TickTime < 0.f)
		return;

	AccumulatedTickTime += InDeltaTime;

	if (AccumulatedTickTime >= TickTime)
	{
		AccumulatedTickTime -= TickTime;
		TickBossUI(MyGeometry, TickTime);
	}
}

void UMMOBossUI::TickBossUI(const FGeometry& MyGeometry, float InElapsedTime)
{
	if (CharacterReference)
	{
		SetHealth(CharacterReference->GetHealthPercent());
		SetMana(CharacterReference->GetResourcePercent());
	}
}
