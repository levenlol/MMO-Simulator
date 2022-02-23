// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Core/MMOCommon.h"
#include "GameplayTagContainer.h"
#include "Containers/Map.h"
#include "MMOBaseCharacter.generated.h"

class AMMOBaseWeapon;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMMOOnDamageTaken, AMMOBaseCharacter*, Sender, FMMODamage, Damage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMMOStartAttack, AMMOBaseCharacter*, Sender);


UCLASS()
class MMO_SIMULATOR_API AMMOBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMMOBaseCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	FName Name;

	// Class of the character
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	EMMOCharacterClass CharacterClass;

	// How happy is the character.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	int32 Happiness;

	// How greedy is the character
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	int32 Greediness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	FName MainHandSocket = ("hand_r");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	FName OffHandSocket = ("hand_l");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	FMMOCharacterAttributes Attributes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	FMMOCharacterStats Stats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Resistances)
	FMMOResistances Resistances;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int32 Gold;

	UFUNCTION(BlueprintPure, Category = Stats)
	FORCEINLINE float GetHealthPercent() const { return static_cast<float>(Stats.Health) / static_cast<float>(Stats.MaxHealth); }

	UFUNCTION(BlueprintPure, Category = Stats)
	FORCEINLINE float GetResourcePercent() const { return static_cast<float>(Stats.Resources) / static_cast<float>(Stats.MaxResources); }

	UFUNCTION(BlueprintPure, Category = Stats)
	FORCEINLINE AMMOBaseWeapon* GetMainHandWeapon() const { return MainHandWeapon; }

	UFUNCTION(BlueprintPure, Category = Stats)
	FORCEINLINE AMMOBaseWeapon* GetOffHandWeapon() const { return OffHandWeapon; }

	UFUNCTION(BlueprintPure, Category = Stats)
	bool IsAttacking() const;

	UFUNCTION(BlueprintPure, Category = Stats)
	bool IsStunned() const;

	UFUNCTION(BlueprintPure, Category = Stats)
	bool CanCharacterAttack() const;

	UFUNCTION(BlueprintPure, Category = Stats)
	FMMOWeaponTypeCouple GetCurrentEquippedWeaponsType() const;

	UFUNCTION(BlueprintNativeEvent, Category = Damage)
	void DamageTake(FMMODamage InDamage);

	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool TryEquipWeapon(TSubclassOf<AMMOBaseWeapon> InWeaponClass, bool bMainHand);

	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool StartAttack(AMMOBaseCharacter* Target);

	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool CanAttackTarget(AMMOBaseCharacter* Target) const;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Events
	UPROPERTY(BlueprintAssignable, Category = Combat)
	FMMOOnDamageTaken OnDamageTaken;

	UPROPERTY(BlueprintAssignable, Category = Combat)
	FMMOStartAttack OnStartAttack;

	UPROPERTY(VisibleAnywhere, Category = Status)
	FGameplayTagContainer StatusTags;

	UPROPERTY(EditAnywhere, Category = Status)
	FGameplayTag AttackTag;

	UPROPERTY(EditAnywhere, Category = Status)
	FGameplayTag StunnedTag;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	bool TryAttack(AMMOBaseCharacter* Target);
	void StopAttack();

	FMMODamage ComputeAutoAttackDamage() const;

private:
	FTimerHandle RecuperateTimerHandle;

	void OnRecuperate();

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AMMOBaseWeapon* MainHandWeapon;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AMMOBaseWeapon* OffHandWeapon;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	float LastAttackTime = 0.f;
};
