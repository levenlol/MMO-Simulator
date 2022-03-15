// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Core/MMOCommon.h"
#include "GameplayTagContainer.h"
#include "Containers/Map.h"
#include "Tools/MMOMathExpression.h"
#include "GameplayTagsManager.h"
#include "MMOBaseCharacter.generated.h"

class AMMOBaseWeapon;
class UMMOCombatSystem;
class UMMOStatsManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMMOOnCharacterEvent, AMMOBaseCharacter*, Sender);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMMOOnDamageTaken, AMMOBaseCharacter*, Sender, FMMODamage, Damage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FMMOOnEquipWeapon, AMMOBaseCharacter*, Sender, AMMOBaseWeapon*, NewWeapon, AMMOBaseWeapon*, OldWeapon);


struct MMO_SIMULATOR_API FMMOStatusTags : public FGameplayTagNativeAdder
{
	FGameplayTag RootedTag;
	FGameplayTag StunnedTag;
	FGameplayTag SlowedTag;
	FGameplayTag AttackTag;
	FGameplayTag CastTag;

	FORCEINLINE static const FMMOStatusTags& Get() { return StatusTags; }

protected:
	virtual void AddTags() override
	{
		UGameplayTagsManager& Manager = UGameplayTagsManager::Get();

		RootedTag = Manager.AddNativeGameplayTag(FName("Status.Malus.Rooted"));
		StunnedTag = Manager.AddNativeGameplayTag(FName("Status.Malus.Stunned"));
		SlowedTag = Manager.AddNativeGameplayTag(FName("Status.Malus.Slowed"));
		AttackTag = Manager.AddNativeGameplayTag(FName("Status.Action.Attack"));
		CastTag = Manager.AddNativeGameplayTag(FName("Status.Action.Cast"));
	}
private:
	static FMMOStatusTags StatusTags;
};

UCLASS()
class MMO_SIMULATOR_API AMMOBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMMOBaseCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat)
	UMMOCombatSystem* CombatSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Stats)
	UMMOStatsManager* StatsManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Info)
	FMMOCharacter CharacterInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	FName MainHandSocket = ("hand_r");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	FName OffHandSocket = ("hand_l");

	UFUNCTION(BlueprintPure, Category = Stats)
	FORCEINLINE float GetHealthPercent() const { return static_cast<float>(CharacterInfo.Stats.Health) / static_cast<float>(CharacterInfo.Stats.MaxHealth); }

	UFUNCTION(BlueprintPure, Category = Stats)
	FORCEINLINE float GetResourcePercent() const { return static_cast<float>(CharacterInfo.Stats.Resources) / static_cast<float>(CharacterInfo.Stats.MaxResources); }

	UFUNCTION(BlueprintPure, Category = Stats)
	FORCEINLINE AMMOBaseWeapon* GetMainHandWeapon() const { return MainHandWeapon; }

	UFUNCTION(BlueprintPure, Category = Stats)
	FORCEINLINE AMMOBaseWeapon* GetOffHandWeapon() const { return OffHandWeapon; }

	UFUNCTION(BlueprintPure, Category = Stats)
	FMMOWeaponTypeCouple GetCurrentEquippedWeaponsType() const;

	UFUNCTION(BlueprintCallable, Category = Character)
	void InitializeCharacter(const FMMOCharacter& InCharacter);

	UFUNCTION(BlueprintNativeEvent, Category = Damage)
	void DamageTake(FMMODamage InDamage);

	// Get minimum range of current equipped weapon
	UFUNCTION(BlueprintPure, Category = Weapon)
	float GetWeaponRange() const;

	// Return if any weapon is equipped (only main hand weapon is checked).
	UFUNCTION(BlueprintPure, Category = Weapon)
	bool HasEquippedAnyWeapons() const;

	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool TryEquipWeapon(TSubclassOf<AMMOBaseWeapon> InWeaponClass, bool bMainHand);

	UFUNCTION(BlueprintCallable, Category = Status)
	void GiveTag(const FGameplayTag& InTag);

	UFUNCTION(BlueprintCallable, Category = Status)
	void RemoveTag(const FGameplayTag& InTag);

	UFUNCTION(BlueprintPure, Category = Status)
	bool HasTag(const FGameplayTag& InTag) const;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Events
	UPROPERTY(BlueprintAssignable, Category = Combat)
	FMMOOnCharacterEvent OnCharacterInitialized;

	UPROPERTY(BlueprintAssignable, Category = Combat)
	FMMOOnCharacterEvent OnCharacterStunned;

	UPROPERTY(BlueprintAssignable, Category = Combat)
	FMMOOnDamageTaken OnDamageTaken;

	UPROPERTY(BlueprintAssignable, Category = Weapon)
	FMMOOnEquipWeapon OnEquipWeapon;

	UPROPERTY(VisibleAnywhere, Category = Status)
	FGameplayTagContainer StatusTags;

	// TODO: More structured Character Stats initialization.
	UPROPERTY(EditDefaultsOnly, Category = Initialization)
	FMMOMathExpression HealthExpression;

	UPROPERTY(EditDefaultsOnly, Category = Initialization)
	FMMOMathExpression ManaExpression;

	UFUNCTION(BlueprintCallable, Category = Status)
	void Stun(float InDuration);

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintNativeEvent, Category = Status)
	void Die();

private:
	FTimerHandle RecuperateTimerHandle;

	void OnRecuperate();

	UPROPERTY(VisibleAnywhere, Transient, Category = Weapon)
	AMMOBaseWeapon* MainHandWeapon;

	UPROPERTY(VisibleAnywhere, Transient, Category = Weapon)
	AMMOBaseWeapon* OffHandWeapon;

	UPROPERTY(VisibleAnywhere, Transient, Category = Weapon)
	float StunTimerRemaining = 0.f;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	bool bAlive = true;
};
