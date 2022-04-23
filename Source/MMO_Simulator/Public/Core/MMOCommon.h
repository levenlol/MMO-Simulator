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
enum class EMMOCharacterRace : uint8
{
	Human,
	Elf,
	Dwarf,
	Gnome,
	Fairy,
	Orc,
	Goblin,
	Troll,
	Minotaur,
	Reptilian, // Trump
	Draconic,
	Ogre,
	Undead
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
	Cloth,
	Leather,
	Mail,
	Plate
};

UENUM(BlueprintType)
enum class EMMOArmorSlotType : uint8
{
	Head,
	Shoulder,
	Arms,
	Chest,
	Legs,
	Feets,
	Ring,
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
	FMMOCharacterAttributes()
	{
		Strength = 0;
		Constitution = 0;
		Intellect = 0;
		Dexterity = 0;
	}

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

	FMMOCharacterAttributes operator+(const FMMOCharacterAttributes& Other) const
	{
		FMMOCharacterAttributes ReturnAttributes = *this;
		ReturnAttributes.Strength += Other.Strength;
		ReturnAttributes.Intellect += Other.Intellect;
		ReturnAttributes.Constitution += Other.Constitution;
		ReturnAttributes.Dexterity += Other.Dexterity;

		return ReturnAttributes;
	}
};

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMOCombatAttributes
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attributes)
	int32 SpellCritChanceRating = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attributes)
	int32 AttackCritChanceRating = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attributes)
	int32 DodgeRating = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attributes)
	int32 BlockRating = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attributes)
	int32 AttackPower = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attributes)
	int32 SpellPower = 0;

	FMMOCombatAttributes operator+(const FMMOCombatAttributes& Other) const
	{
		FMMOCombatAttributes ReturnAttributes = *this;
		ReturnAttributes.SpellCritChanceRating += Other.SpellCritChanceRating;
		ReturnAttributes.AttackCritChanceRating += Other.AttackCritChanceRating;
		ReturnAttributes.DodgeRating += Other.DodgeRating;
		ReturnAttributes.BlockRating += Other.BlockRating;
		ReturnAttributes.AttackPower += Other.AttackPower;
		ReturnAttributes.SpellPower += Other.SpellPower;

		return ReturnAttributes;
	}
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
	int32 HPS = 0;

	// Resources per seconds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int32 RPS = 0;

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
	FMMOCharacter() = default;
	FMMOCharacter(const FName& InName);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	FName Name;

	// Class of the character
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	EMMOCharacterClass CharacterClass;

	// Race of the character
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	EMMOCharacterRace Race;

	// Role of the character
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	EMMOCharacterRole CharacterRole;

	// How happy is the character.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	int32 Happiness;

	// How greedy the character is.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	int32 Greediness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats, meta=(ClampMin="1"))
	int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	FMMOCharacterStats Stats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Resistances)
	FMMOResistances Resistances;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Attributes)
	FMMOCharacterAttributes Attributes;

	// Initial attributes of the character, (The random generated one).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attributes)
	FMMOCharacterAttributes InitialAttributes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	FMMOCombatAttributes CombatAttributes;

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
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	FMMOCharacterAttributes Attributes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	FMMOCombatAttributes CombatAttributes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	FMMOResistances Resistances;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	EMMORarityType Rarity = EMMORarityType::Common;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	EMMOArmorSlotType ArmorSlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	EMMOArmorType ArmorType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int32 ItemLevel = 1;

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
	int32 DamageMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int32 DamageMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	float WeaponRange = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats, meta = (ClampMin = "0.5"))
	float AttackSpeed = 2.f;
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

	UFUNCTION(BlueprintPure, Category = Utility, meta = (WorldContext = WorldContextObject))
	static FVector ProjectPointToTerrain(const UObject* WorldContextObject, const FVector& InLocation, float RayLength, ECollisionChannel CollisionChannel, float UpRayOffset= 0.f);
};