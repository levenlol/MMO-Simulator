// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/MMOCommon.h"
#include "Tools/MMOMathExpression.h"
#include "MMOStatsManager.generated.h"

class AMMOBaseCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MMO_SIMULATOR_API UMMOStatsManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMMOStatsManager();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// compute attributes and combat attributes and then set it to the character.
	UFUNCTION(BlueprintCallable, Category = Stats)
	void UpdateCharacterAttributes(const FMMOCharacterAttributes& InAttributes, const FMMOCombatAttributes& InCombatAttributes);

	UFUNCTION(BlueprintCallable, Category = Stats)
	void RecomputeCombatAttributesChances();

	UFUNCTION(BlueprintCallable, Category = Stats)
	void OnLevelUp();

	UPROPERTY(EditAnywhere, Category = Attributes)
	FMMOMathExpression ParryExpression;

	UPROPERTY(EditAnywhere, Category = Attributes)
	FMMOMathExpression BlockExpression;

	UPROPERTY(EditAnywhere, Category = Attributes)
	FMMOMathExpression DodgeExpression;

	UPROPERTY(EditAnywhere, Category = Attributes)
	FMMOMathExpression AttackCritExpression;

	UPROPERTY(EditAnywhere, Category = Attributes)
	FMMOMathExpression SpellCritExpression;

protected:
	UFUNCTION()
	void OnCharacterInitialized(AMMOBaseCharacter* Sender);

	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;

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

	UPROPERTY()
	AMMOBaseCharacter* OwnerCharacter;

	UPROPERTY()
	FMMOCharacter BaseCharacterInfo;
};
