// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MMOCommon.h"
#include "Data/MMODataFinder.h"
#include "Items/MMOBaseWeapon.h"
#include "Characters/MMOBaseCharacter.h"
#include "Characters/MMOBaseEnemy.h"
#include "Characters/MMOBaseHero.h"
#include "Core/MMOGameState.h"
#include "Components/Button.h"
#include "Engine/StreamableManager.h"
#include "Data/MMODataFinder.h"
#include "Kismet/GameplayStatics.h"


void FMMOCharacterStats::Recuperate(int32 ElapsedSeconds)
{
	Health = FMath::Clamp(Health + ElapsedSeconds * HPS, 0, MaxHealth);
	Resources = FMath::Clamp(Resources + ElapsedSeconds * RPS, 0, MaxResources);
}

bool UMMOGameplayUtils::Is2HWeapon(EMMOWeaponType WeaponType)
{
	switch (WeaponType)
	{
	case EMMOWeaponType::Axe2h:
	case EMMOWeaponType::Sword2h:
	case EMMOWeaponType::Mace2h:
	case EMMOWeaponType::Bow:
	case EMMOWeaponType::Rifle:
	case EMMOWeaponType::Spear:
	case EMMOWeaponType::Staff:
		return true;
	default:
		break;
	}

	return false;
}

UAnimSequenceBase* UMMOGameplayUtils::GetAnimSequence(const FMMOWeaponTypeCouple& WeaponCouple)
{
	UMMODataFinder* DataFinder = UMMODataFinder::Get();
	return DataFinder->GetAnimSequence(WeaponCouple);
}

float UMMOGameplayUtils::PlayRateToFitAnimation(const UAnimSequenceBase* Anim, float TargetSeconds)
{
	if (Anim && !FMath::IsNearlyZero(TargetSeconds, KINDA_SMALL_NUMBER))
	{
		return Anim->GetPlayLength() / TargetSeconds;
	}

	UE_LOG(LogTemp, Error, TEXT("Cannot Fit Animation %s to seconds %f"), Anim ? *Anim->GetName() : TEXT("NULL"), TargetSeconds);
	return 1.0f;
}

const TArray<AMMOBaseHero*>& UMMOGameplayUtils::GetHeroes(const UObject* WorldContextObject)
{
	if (AMMOGameState* GameState = AMMOGameState::GetMMOGameState(WorldContextObject))
	{
		return GameState->Heroes;
	}

	static TArray<AMMOBaseHero*> NoHeroes;
	return NoHeroes;
}

void UMMOGameplayUtils::SetActorActive(AActor* InActor, bool bActive)
{
	if (InActor)
	{
		InActor->SetActorHiddenInGame(!bActive);
		InActor->SetActorEnableCollision(bActive);
		InActor->SetActorTickEnabled(bActive);
	}
}

void UMMOGameplayUtils::AsyncChangeImageButton(UButton* Button, TSoftObjectPtr<UTexture2D> Texture)
{
	if (!Button || Texture.IsNull())
	{
		return;
	}
	
	static FStreamableManager AssetLoader;

	TWeakObjectPtr<UButton> WeakButton = Button;
	AssetLoader.RequestAsyncLoad(Texture.ToSoftObjectPath(), FStreamableDelegate::CreateLambda([WeakButton, Texture]() 
		{
			if (WeakButton.IsValid())
			{
				FButtonStyle Style;
				Style.Normal.SetResourceObject(Texture.Get());
				Style.Disabled.SetResourceObject(Texture.Get());
				Style.Hovered.SetResourceObject(Texture.Get());
				Style.Pressed.SetResourceObject(Texture.Get());

				WeakButton->SetStyle(Style);
			}
		}));
}

bool UMMOGameplayUtils::AreOnTheSameSide(AMMOBaseCharacter* First, AMMOBaseCharacter* Second)
{
	if (!First || !Second)
		return false;

	return First->IsA<AMMOBaseHero>() == Second->IsA<AMMOBaseHero>();
}

TArray<FName> UMMOGameplayUtils::GetKeysForAction(const UObject* WorldContextObject, const FName& InputName, int32 PlayerIndex)
{
	static const TMap<FName, FName> TranslationUnit = { 
		{"Zero", "0"}, 
		{"One", "1"},
		{"Two", "2"},
		{"Three", "3"},
		{"Four", "4"},
		{"Five", "5"},
		{"Six", "6"},
		{"Seven", "7"},
		{"Eigth", "8"},
		{"Nine", "9"},
	};
 
	TArray<FName> Names;
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, PlayerIndex))
	{
		const TArray<FInputActionKeyMapping>& ActionKeys = PlayerController->PlayerInput->GetKeysForAction(InputName);

		for (const FInputActionKeyMapping& Key : ActionKeys)
		{
			FName Name = Key.Key.GetFName();

			if (TranslationUnit.Contains(Name))
			{
				Name = TranslationUnit[Name];
			}

			Names.Add(Name);
		}
	}

	return Names;
}

FMMOCharacter::FMMOCharacter(const FName& InName)
	: Name(InName)
{}
