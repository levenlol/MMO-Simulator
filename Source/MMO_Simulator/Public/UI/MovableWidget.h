// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MovableWidget.generated.h"

class UButton;

UCLASS()
class MMO_SIMULATOR_API UMovableWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget, meta = (BindWidget))
	UButton* TopWindowBar;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget, meta = (BindWidget))
	UButton* CloseWindowButton;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UFUNCTION()
	void TopWindowBarPressed();

	UFUNCTION()
	void TopWindowBarReleased();

	UFUNCTION()
	void CloseClicked();

	bool bMoving = false;

	FVector2D ClickOffset = FVector2D::ZeroVector;
};
