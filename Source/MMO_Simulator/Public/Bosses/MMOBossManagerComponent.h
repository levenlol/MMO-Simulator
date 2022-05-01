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
class UMMOBossUI;

DECLARE_DELEGATE_RetVal_OneParam(bool, FBossConditionEventDelegate, AMMOBaseBoss*);
DECLARE_DELEGATE_OneParam(FBossCallbackEventDelegate, AMMOBaseBoss*)

struct MMO_SIMULATOR_API FMMOEventDelegateHelper
{
	/** Holds the delegate to call. */
	FBossConditionEventDelegate ConditionFuncDelegate;
	FBossCallbackEventDelegate CallbackFuncDelegate;

	FMMOEventDelegateHelper() {};
	FMMOEventDelegateHelper(FBossConditionEventDelegate const& Condition, FBossCallbackEventDelegate const& Callback)
		: ConditionFuncDelegate(Condition)
		, CallbackFuncDelegate(Callback)
	{};

	inline bool Execute_Condition(AMMOBaseBoss* Boss)
	{
		check(Boss);
		if (ConditionFuncDelegate.IsBound())
		{
			return ConditionFuncDelegate.Execute(Boss);
		}

		return false;
	}

	inline void Execute_Callback(AMMOBaseBoss* Boss)
	{
		check(Boss);
		if (CallbackFuncDelegate.IsBound())
		{
			CallbackFuncDelegate.Execute(Boss);
		}
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MMO_SIMULATOR_API UMMOBossManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMMOBossManagerComponent();

	UFUNCTION(BlueprintPure, Category = Phase)
	FORCEINLINE int32 GetCurrentPhase() const { return CurrentPhase; }

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	template<class UserClass>
	void AddCustomBossEvent(UserClass* InObj, typename FBossConditionEventDelegate::TMethodPtr<UserClass> InCondition, typename FBossCallbackEventDelegate::TMethodPtr<UserClass> InCallback)
	{
		FMMOEventDelegateHelper DelegateHelper(
			FBossConditionEventDelegate::CreateUObject(InObj, InCondition),
			FBossCallbackEventDelegate::CreateUObject(InObj, InCallback));

		PendingEvents.Add(MoveTemp(DelegateHelper));
	}

	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<UMMOBossUI> BossWidgetClass;

	void ShowBossWidget();
	void HideBossWidget();

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

	TArray<FMMOEventDelegateHelper> PendingEvents;

	void CheckEvents();

	UPROPERTY(VisibleInstanceOnly, Category = UI)
	UMMOBossUI* BossWidget = nullptr;
};
