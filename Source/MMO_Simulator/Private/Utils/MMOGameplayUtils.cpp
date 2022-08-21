#include "Utils/MMOGameplayUtils.h"
#include "Data/MMODataFinder.h"
#include "Items/MMOBaseWeapon.h"
#include "Characters/MMOBaseEnemy.h"
#include "Characters/MMOBaseHero.h"
#include "Core/MMOGameState.h"
#include "Components/Button.h"
#include "Engine/StreamableManager.h"
#include "Data/MMODataFinder.h"
#include "Kismet/GameplayStatics.h"
#include "CombatSystem/Skills/MMOBaseSkill.h"
#include "Particles/ParticleSystemComponent.h"


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

void UMMOGameplayUtils::SetImageToButton(UButton* InButton, UTexture* InTexture)
{
	if (InButton && InTexture)
	{
		InButton->WidgetStyle.Normal.SetResourceObject(InTexture);
		InButton->WidgetStyle.Hovered.SetResourceObject(InTexture);
		InButton->WidgetStyle.Pressed.SetResourceObject(InTexture);
		InButton->WidgetStyle.Disabled.SetResourceObject(InTexture);
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

int32 UMMOGameplayUtils::GetResistanceFromType(AMMOBaseCharacter* Character, EMMODamageType DamageType)
{
	if(Character)
	{
		switch (DamageType)
		{
		case EMMODamageType::Physical:
			return Character->CharacterInfo.Resistances.Armor;
		case EMMODamageType::Fire:
			return Character->CharacterInfo.Resistances.FireResistance;
		case EMMODamageType::Ice:
			return Character->CharacterInfo.Resistances.IceResistance;
		case EMMODamageType::Shadow:
			return Character->CharacterInfo.Resistances.ShadowResistance;
		case EMMODamageType::Holy:
			return Character->CharacterInfo.Resistances.HolyResistance;
		case EMMODamageType::Nature:
			return Character->CharacterInfo.Resistances.NatureResistance;
		case EMMODamageType::Arcane:
			return Character->CharacterInfo.Resistances.ArcaneResistance;
		}
	}

	UE_LOG(LogTemp, Error, TEXT("GetResistanceFromType failed to get a value."));
	return 0;
}

float UMMOGameplayUtils::GetDamageReduction(AMMOBaseCharacter* Defender, AMMOBaseCharacter* Attacker, EMMODamageType DamageType)
{
	const float AttackerLevel = Attacker ? static_cast<float>(Attacker->CharacterInfo.Level) : 1.f;

	const float ResistanceValue = static_cast<float>(GetResistanceFromType(Defender, DamageType));

	if (DamageType == EMMODamageType::Physical)
	{
		// https://wowpedia.fandom.com/wiki/Armor
		return ResistanceValue / (ResistanceValue + 400.f + 85.f * AttackerLevel);
	}
	else
	{
		// https://wowpedia.fandom.com/wiki/Resistance
		return ResistanceValue / (ResistanceValue + 150.f);
	}
}

void UMMOGameplayUtils::PlayParticlesAt(AMMOFXActor* FxActor, const FVector& Location)
{
	if (FxActor)
	{
		FxActor->SetActorLocation(Location);
		FxActor->SetActorHiddenInGame(false);

		FxActor->ParticleComponent->KillParticlesForced();
		FxActor->ParticleComponent->ResetParticles();
		FxActor->ParticleComponent->ActivateSystem();
	}
}

FString UMMOGameplayUtils::GetClassName(EMMOCharacterClass InClass)
{
	static UEnum* ClassEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EMMOCharacterClass"));
	if (!ClassEnum)
	{
		UE_LOG(LogTemp, Fatal, TEXT("Cannot find EMMOCharacterClass"));
		return "";
	}

	return ClassEnum->GetNameStringByValue(static_cast<int64>(InClass));
}

FVector UMMOGameplayUtils::ComputeHeroesSelectionMiddlePoint(const TArray<AMMOBaseHero*>& Heroes)
{
	TArray<FVector> Positions;
	Algo::Transform(Heroes, Positions, [](const AMMOBaseHero* Hero) { return Hero->GetActorLocation(); });
	
	return ComputeMiddlePoint(Positions);
}

FVector UMMOGameplayUtils::ComputeMiddlePoint(const TArray<FVector>& Points)
{
	float X = 0.f;
	float Y = 0.f;
	float Z = 0.f;

	// online-average
	for (int32 i = 0; i < Points.Num(); i++)
	{
		const FVector& Location = Points[i];
		const float n = (i + 1.f);
		const float a = 1 / n;
		const float b = 1 - a;

		X = a * Location.X + b * X;
		Y = a * Location.Y + b * Y;
		Z = a * Location.Z + b * Z;
	}

	return FVector(X, Y, Z);
}

TArray<AMMOBaseHero*> UMMOGameplayUtils::FilterByRole(TArray<AMMOBaseHero*> Heroes, EMMOCharacterRole Role)
{
	return Heroes.FilterByPredicate([Role](const AMMOBaseHero* Hero) { return Hero && Hero->CharacterInfo.CharacterRole == Role; });
}
