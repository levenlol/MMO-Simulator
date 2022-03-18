// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/MMOCommon.h"
#include "Tools/MMOMathExpression.h"
#include "MMOStatsManager.generated.h"

class AMMOBaseCharacter;

UCLASS(BlueprintType, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MMO_SIMULATOR_API UMMOStatsManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMMOStatsManager();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = Stats)
	void RecomputeCombatAttributesChances();

	UFUNCTION(BlueprintCallable, Category = Stats)
	void ComputeAttributes();

	UFUNCTION(BlueprintCallable, Category = Stats)
	void LevelUp();

	// return stats of the character at current level ( equip stats excluded )
	UFUNCTION(BlueprintPure, Category = Stats)
	const FMMOCharacterAttributes& GetCharacterNakedAttributes() const { return BaseAttributes; }

	UPROPERTY(EditAnywhere, Category = Attributes)
	FMMOMathExpression ParryExpression;

	UPROPERTY(EditAnywhere, Category = Attributes)
	FMMOMathExpression BlockExpression;

	UPROPERTY(EditAnywhere, Category = Attributes)
	FMMOMathExpression DodgeExpression;

	UPROPERTY(EditAnywhere, Category = Attributes)
	FMMOMathExpression AttackPowerExpression;

	UPROPERTY(EditAnywhere, Category = Attributes)
	FMMOMathExpression SpellPowerExpression;

	UPROPERTY(EditAnywhere, Category = Attributes)
	FMMOMathExpression AttackCritExpression;

	UPROPERTY(EditAnywhere, Category = Attributes)
	FMMOMathExpression SpellCritExpression;

	UPROPERTY(EditAnywhere, Category = Initialization)
	FMMOMathExpression HealthExpression;

	UPROPERTY(EditAnywhere, Category = Initialization)
	FMMOMathExpression ManaExpression;

protected:
	UFUNCTION()
	void OnCharacterInitialized(AMMOBaseCharacter* Sender);

	virtual void BeginPlay() override;
	virtual void OnRegister() override;
	virtual void OnUnregister() override;

	UPROPERTY(VisibleAnywhere, Category = CombatAttributes)
	float ParryChance;

	UPROPERTY(VisibleAnywhere, Category = CombatAttributes)
	float BlockChance;

	UPROPERTY(VisibleAnywhere, Category = CombatAttributes)
	float DodgeChance;

	UPROPERTY(VisibleAnywhere, Category = CombatAttributes)
	float AttackCritChance;

	UPROPERTY(VisibleAnywhere, Category = CombatAttributes)
	float SpellCritChance;

	UPROPERTY(VisibleAnywhere, Category = CombatAttributes)
	int32 SpellPowerValue;

	UPROPERTY(VisibleAnywhere, Category = CombatAttributes)
	int32 AttackPowerValue;

	UPROPERTY()
	AMMOBaseCharacter* OwnerCharacter;

	UPROPERTY(VisibleAnywhere, Category = Stats)
	FMMOCharacterAttributes BaseAttributes;

	void RecomputeHealthAndResources();

	// compute attributes and combat attributes and then set it to the character.
	void UpdateCharacterAttributes();
};
