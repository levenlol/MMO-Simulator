// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AI/MMOAIController.h"
#include "Characters/MMOBaseBoss.h"

#include "MMOBossManagerComponent.generated.h"

class UMMOAggroManagerComponent;
class AMMOBaseCharacter;
class UBehaviorTree;

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMOHealthPercentEventData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = Data, meta=(ClampMin ="0.0", ClampMax = "100.0"))
	int32 HealthPercent = 0;

	UPROPERTY(EditAnywhere, Category = Data)
	UBehaviorTree* NewBT = nullptr;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MMO_SIMULATOR_API UMMOBossManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMMOBossManagerComponent();

	// Events fired when boss reach a pre-determined health percent value.
	UPROPERTY(EditDefaultsOnly, Category = Setup)
	TArray<FMMOHealthPercentEventData> DropHealthPercentEvents;

	UFUNCTION(BlueprintNativeEvent, Category = Phase)
	void OnDropHealthEvent(const FMMOHealthPercentEventData& HealthPercentData);

	UFUNCTION(BlueprintPure, Category = Phase)
	FORCEINLINE int32 GetCurrentPhase() const { return CurrentPhase; }

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category = Data)
	int32 GetBossHealthPercent() const;

	FORCEINLINE AMMOBaseBoss* GetBossPawn() const { return Cast<AMMOBaseBoss>(GetOwner()); }
	FORCEINLINE AMMOAIController* GetAIController() const { return Cast<AMMOAIController>(GetBossPawn()->GetController()); }
private:

	UFUNCTION()
	void OnEnterCombat(UMMOAggroManagerComponent* Sender, AMMOBaseCharacter* Danger);

	UPROPERTY(VisibleAnywhere, Category = Phase)
	int32 CurrentPhase = 0;

	void CheckPhase();
};
