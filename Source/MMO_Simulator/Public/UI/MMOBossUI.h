// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MMOBossUI.generated.h"

class UImage;
class AMMOBaseCharacter;
class UTextBlock;

UCLASS()
class MMO_SIMULATOR_API UMMOBossUI : public UUserWidget
{
	GENERATED_BODY()
public:
	UMMOBossUI(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = Setup)
	FName HealthParameterName;

	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = Setup)
	FName ManaParameterName;

	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = Setup)
	FName CastParameterName;

	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = Setup)
	float TickTime = 0.f;

	UPROPERTY(meta = (BindWidget))
	UImage* HealthBar;

	UPROPERTY(meta = (BindWidget, OptionalWidget = true))
	UImage* ManaBar;
	
	UPROPERTY(meta = (BindWidget, OptionalWidget = true))
	UTextBlock* BossNameTextBlock;

	UPROPERTY(meta = (BindWidget, OptionalWidget = true))
	UImage* CastBar;

	UPROPERTY(meta = (BindWidget, OptionalWidget = true))
	UTextBlock* AbilityNameTextBlock;

	UFUNCTION(BlueprintCallable, Category = Resources)
	void SetHealth(float InPercent);

	UFUNCTION(BlueprintCallable, Category = Resources)
	void SetMana(float InPercent);

	UFUNCTION(BlueprintCallable, Category = Resources)
	void SetName(const FString& InName);

	UFUNCTION(BlueprintCallable, Category = Resources)
	void SetCharacter(AMMOBaseCharacter* InCharacter);

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override final;

	virtual void TickBossUI(const FGeometry& MyGeometry, float InElapsedTime);

private:
	UPROPERTY()
	AMMOBaseCharacter* CharacterReference;

	void HandleCastBar();

	float AccumulatedTickTime = 0.f;
};
