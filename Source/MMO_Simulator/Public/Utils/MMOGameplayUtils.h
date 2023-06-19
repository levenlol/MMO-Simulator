
#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Characters/MMOBaseCharacter.h"
#include "MMOGameplayUtils.generated.h"

class UAnimSequenceBase;
class AMMOBaseCharacter;
class AMMOBaseHero;
class UButton;

/** Static class with useful utility functions that can be called from both Blueprint and C++ */
UCLASS()
class MMO_SIMULATOR_API UMMOGameplayUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = Weapon)
	static bool Is2HWeapon(EMMOWeaponType WeaponType);

	UFUNCTION(BlueprintPure, Category = Animation, meta = (BlueprintThreadSafe))
	static UAnimSequenceBase* GetAnimSequence(const FMMOWeaponTypeCouple& WeaponCouple);

	UFUNCTION(BlueprintPure, Category = Animation, meta = (BlueprintThreadSafe))
	static float PlayRateToFitAnimation(const UAnimSequenceBase* Anim, float TargetSeconds);
	
	UFUNCTION(BlueprintPure, Category = Heroes, meta = (WorldContext = WorldContextObject))
	static const TArray<AMMOBaseHero*>& GetHeroes(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = Heroes, meta = (WorldContext = WorldContextObject))
	static TArray<AMMOBaseHero*> GetHeroesOfRole(const UObject* WorldContextObject, EMMOGuildRole Role);

	UFUNCTION(BlueprintPure, Category = Heroes, meta = (WorldContext = WorldContextObject))
	static AMMOBaseHero* GetRandomGuildHero(const UObject* WorldContextObject, const bool bExcludeGM = false);

	UFUNCTION(BlueprintCallable, Category = Heroes, meta = (WorldContext = WorldContextObject))
	static void SetActorActive(AActor* InActor, bool bActive);

	UFUNCTION(BlueprintCallable, Category = UI)
	static void SetImageToButton(UButton* InButton, UTexture* InTexture);
	
	UFUNCTION(BlueprintCallable, Category = Widget, meta = (WorldContext = WorldContextObject))
	static void AsyncChangeImageButton(UButton* Button, TSoftObjectPtr<UTexture2D> Texture);

	UFUNCTION(BlueprintCallable, Category = Combat)
	static bool AreOnTheSameSide(AMMOBaseCharacter* First, AMMOBaseCharacter* Second);

	UFUNCTION(BlueprintPure, Category = UI, meta = (WorldContext = WorldContextObject))
	static TArray<FName> GetKeysForAction(const UObject* WorldContextObject, const FName& InputName, int32 PlayerIndex = 0);

	UFUNCTION(BlueprintPure, Category = Stats)
	static int32 GetResistanceFromType(AMMOBaseCharacter* Character, EMMODamageType DamageType);

	UFUNCTION(BlueprintCallable, Category = FX)
	static void PlayParticlesAt(AMMOFXActor* FxActor, const FVector& Location);

	UFUNCTION(BlueprintPure, Category = FX)
	static float GetDamageReduction(AMMOBaseCharacter* Defender, AMMOBaseCharacter* Attacker, EMMODamageType DamageType);
		
	UFUNCTION(BlueprintPure, Category = Utility)
	static FString GetClassName(EMMOCharacterClass InClass);

	UFUNCTION(BlueprintPure, Category = Utility)
	static FVector ComputeHeroesSelectionMiddlePoint(const TArray<AMMOBaseHero*>& Heroes);

	UFUNCTION(BlueprintPure, Category = Utility)
	static FVector ComputeMiddlePoint(const TArray<FVector>& Points);

	UFUNCTION(BlueprintPure, Category = Utility)
	static TArray<AMMOBaseHero*> FilterByRole(TArray<AMMOBaseHero*> Heroes, EMMOCharacterRole Role);

	UFUNCTION(BlueprintCallable, Category = Utility, meta = (WorldContext = WorldContextObject))
	static FVector VerticalRaycast(const UObject* WorldContextObject, const FVector& InLocation, float RayLength, ECollisionChannel CollisionChannel, float UpRayOffset = 0.f);

	UFUNCTION(BlueprintCallable, Category = Utility)
	static bool IsInLOS(AMMOBaseCharacter* First, AMMOBaseCharacter* Second, ECollisionChannel CollisionChannel);

	UFUNCTION(BlueprintCallable, Category = Utility, meta=(WorldContext="WorldContextObject"))
	static TArray<AMMOBaseCharacter*> GetAllCharactersOfType(const UObject* WorldContextObject, TSubclassOf<AMMOBaseCharacter> CharacterClass = nullptr);
};