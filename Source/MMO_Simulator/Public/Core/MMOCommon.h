// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MMOCommon.generated.h"

class UAnimSequenceBase;
class AMMOBaseCharacter;
class AMMOBaseHero;
class UButton;

UENUM(BlueprintType)
enum class EMMOCharacterClass : uint8
{
	Warrior,
	Paladin,
	Berserker,
	DeathKnight,
	Hunter,
	Shaman,
	Monk,
	Priest,
	Druid,
	Mage,
	Warlock
};

UENUM(BlueprintType)
enum class EMMOCharacterRole : uint8
{
	Tank,
	Healer,
	Melee,
	Ranged
};

UENUM(BlueprintType)
enum class EMMODamageType : uint8
{
	Physical,
	Fire,
	Ice,
	Shadow,
	Holy,
	Nature,
	Arcane
};

UENUM(BlueprintType)
enum class EMMOWeaponType : uint8
{
	Sword1h,
	Sword2h,
	Axe1h,
	Axe2h,
	Mace1h,
	Mace2h,
	Staff,
	Spear,
	Bow,
	Dagger,
	Rifle,
	Shield,
	Unarmed
};

UENUM(BlueprintType)
enum class EMMOArmorType : uint8
{
	Head,
	Shoulder,
	Chest,
	Legs,
	Feet,
	Trinket
};

UENUM(BlueprintType)
enum class EMMORarityType : uint8
{
	Trash,
	Common,
	Uncommon,
	Rare,
	Epic,
	Legendary
};

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMOCharacterAttributes
{
	GENERATED_BODY()
public:
	// Strength: affects physical damage, block.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	int32 Strength;

	// Increase the Maximus Health of the character.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	int32 Constitution;

	// Affects magical damage and resource regen
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	int32 Intellect;

	// Affects dodge and crit chance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	int32 Dexterity;
};

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMOCharacterStats
{
	GENERATED_BODY()
public:
	// Max Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats, meta = (ClampMin = "1"))
	int32 MaxHealth = 100;
	
	// Current Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int32 Health = 70;

	// Max resources
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats, meta = (ClampMin = "1"))
	int32 MaxResources = 70;

	// Mana, rage, energy...
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int32 Resources = 30;

	// Health per seconds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int32 HPS;

	// Resources per seconds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int32 RPS;

	// Recuperate Health and Resource every X seconds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats, meta = (ClampMin = "0"))
	int32 RecuperateEverySeconds = 1;

	void Recuperate(int32 ElapsedSeconds);
};

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMOResistances
{
	GENERATED_BODY()
public:
	// Physical Resistance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Resistance)
	int32 Armor;

	// Fire Resistance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Resistance)
	int32 FireResistance;

	// Ice Resistance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Resistance)
	int32 IceResistance;

	// Shadow Resistance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Resistance)
	int32 ShadowResistance;

	// Holy Resistance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Resistance)
	int32 HolyResistance;

	// Nature Resistance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Resistance)
	int32 NatureResistance;

	// Arcane Resistance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Resistance)
	int32 ArcaneResistance;
};

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMOCharacter
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	FName Name;

	// Class of the character
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	EMMOCharacterClass CharacterClass;

	// Role of the character
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	EMMOCharacterRole CharacterRole;

	// How happy is the character.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	int32 Happiness;

	// How greedy the character is.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	int32 Greediness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	FMMOCharacterAttributes Attributes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	FMMOCharacterStats Stats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Resistances)
	FMMOResistances Resistances;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int32 Gold;
};


USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMODamage
{
	GENERATED_BODY()
public:
	FMMODamage() = default;

	FMMODamage(int32 InDamage, EMMODamageType InDamageType, bool bInCrit, AMMOBaseCharacter* InDamageDealer)
		: Damage(InDamage)
		, DamageType(InDamageType)
		, bCrit(bInCrit) 
		, DamageDealer(InDamageDealer)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
	int32 Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
	EMMODamageType DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
	bool bCrit = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
	AMMOBaseCharacter* DamageDealer;
};

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMOItemStats
{
	GENERATED_BODY()
public:
	FMMOItemStats() = default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	FMMOCharacterAttributes Attributes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	EMMORarityType Rarity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int32 Durability = 100;
};

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMOWeaponStats : public FMMOItemStats
{
	GENERATED_BODY()
public:
	FMMOWeaponStats() = default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	EMMODamageType DamageType = EMMODamageType::Physical;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	FIntPoint Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	float WeaponRange = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats, meta = (ClampMin = "0.5"))
	float AttackSpeed = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats, meta = (ClampMin = "0.5"))
	float CritChance = 0.f;
};

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMOWeaponTypeCouple
{
	GENERATED_BODY()
public:
	FMMOWeaponTypeCouple() {}
	FMMOWeaponTypeCouple(const FMMOWeaponTypeCouple& Other)
		: FMMOWeaponTypeCouple(Other.MainWeaponType, Other.OffWeaponType)
	{}

	FMMOWeaponTypeCouple(const EMMOWeaponType& InMainHandWeaponType, const EMMOWeaponType& InOffHandWeaponType)
	{
		MainWeaponType = InMainHandWeaponType;
		OffWeaponType = InOffHandWeaponType;
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation)
	EMMOWeaponType MainWeaponType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation)
	EMMOWeaponType OffWeaponType;

	bool operator==(const FMMOWeaponTypeCouple& Other) const
	{
		return MainWeaponType == Other.MainWeaponType && OffWeaponType == Other.OffWeaponType;
	}
};

// Type hash for struct.
FORCEINLINE uint32 GetTypeHash(const FMMOWeaponTypeCouple& Thing)
{
	uint32 Hash = FCrc::MemCrc32(&Thing, sizeof(FMMOWeaponTypeCouple));
	return Hash;
}

/** Static class with useful utility functions that can be called from both Blueprint and C++ */
UCLASS()
class MMO_SIMULATOR_API UMMOGameplayUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = Weapon)
	static bool Is2HWeapon(EMMOWeaponType WeaponType);

	UFUNCTION(BlueprintPure, Category = Animation, meta = (BlueprintThreadSafe))
	static UAnimSequenceBase* GetAnimSequence(const FMMOWeaponTypeCouple& WeaponCouple);

	UFUNCTION(BlueprintPure, Category = Animation, meta = (BlueprintThreadSafe))
	static float PlayRateToFitAnimation(const UAnimSequenceBase* Anim, float TargetSeconds);
	
	UFUNCTION(BlueprintPure, Category = Heroes, meta = (WorldContext = WorldContextObject))
	static const TArray<AMMOBaseHero*>& GetHeroes(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = Heroes, meta = (WorldContext = WorldContextObject))
	static void SetActorActive(AActor* InActor, bool bActive);
	
	UFUNCTION(BlueprintCallable, Category = Widget, meta = (WorldContext = WorldContextObject))
	static void AsyncChangeImageButton(UButton* Button, TSoftObjectPtr<UTexture2D> Texture);

	UFUNCTION(BlueprintCallable, Category = Combat, meta = (WorldContext = WorldContextObject))
	static bool AreOnTheSameSide(AMMOBaseCharacter* First, AMMOBaseCharacter* Second);

};