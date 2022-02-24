// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/MMOCommon.h"
#include "MMOAggroManagerComponent.generated.h"

class AMMOBaseCharacter;

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMOAggroData
{
	GENERATED_BODY()
public:
	FMMOAggroData() = default;
	FMMOAggroData(AMMOBaseCharacter* InCharacter, float InAggro = 0.f)
		: Character(InCharacter)
		, Aggro(InAggro)
	{}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Aggro)
	AMMOBaseCharacter* Character;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Aggro)
	float Aggro = 0.f;

	bool operator<(const FMMOAggroData& Other) const { return Aggro < Other.Aggro; }
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MMO_SIMULATOR_API UMMOAggroManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMMOAggroManagerComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Aggro)
	float AggroOnPull = 10.f;

	// Percent for stealing aggro (default is 110 percent).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Aggro)
	float PercentToStealAggro = 1.1f;

	// Damage percent to convert into aggro.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Aggro)
	float DamageAggroPercent = 1.f;

	// Heal percent to convert into aggro.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Aggro)
	float HealAggroPercent = 1.f;

	UFUNCTION(BlueprintCallable, Category = Aggro)
	void AddDanger(AMMOBaseCharacter* InCharacter, float AggroBoost = 0.f);

	UFUNCTION(BlueprintPure, Category = Aggro)
	AMMOBaseCharacter* GetMostDangerousCharacter() const;

	UFUNCTION(BlueprintPure, Category = Aggro)
	const TArray<FMMOAggroData>& GetSortedAggroList() const { return AggroList; }

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
private:
	UFUNCTION()
	void OnTakeDamage(AMMOBaseCharacter* Sender, FMMODamage Damage);

	void SortAggroList();

	// sorted array.highest aggro first
	UPROPERTY(VisibleAnywhere, Category = Aggro)
	TArray<FMMOAggroData> AggroList;
};
