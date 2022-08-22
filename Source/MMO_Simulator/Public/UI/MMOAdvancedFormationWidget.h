// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/MovableWidget.h"
#include "MMOAdvancedFormationWidget.generated.h"

class UMMOFormationManager;
class UButton;
class UCanvasPanel;
class UTexture2D;
struct FMMOFormationSetup;

DECLARE_EVENT_OneParam(UMMOAdvancedFormationButtonWidget, FMMOOnPressed, UMMOAdvancedFormationButtonWidget*)
DECLARE_EVENT_OneParam(UMMOAdvancedFormationButtonWidget, FMMOOnReleased, UMMOAdvancedFormationButtonWidget*)

UCLASS()
class MMO_SIMULATOR_API UMMOAdvancedFormationButtonWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Button;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	FMMOOnPressed& OnPressed() { return PressedEvent; }
	FMMOOnReleased& OnReleased() { return ReleasedEvent; }
private:

	UFUNCTION()
	void OnButtonPressed() { PressedEvent.Broadcast(this); }

	UFUNCTION()
	void OnButtonReleased() { ReleasedEvent.Broadcast(this); }

	FMMOOnPressed PressedEvent;
	FMMOOnReleased ReleasedEvent;
};

UCLASS()
class MMO_SIMULATOR_API UMMOAdvancedFormationWidget : public UMovableWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* TankButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* MeleeButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* RangedButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* HealerButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCanvasPanel* MapCanvas;

	UPROPERTY(EditAnywhere, Category = UI)
	UTexture2D* TankImage;

	UPROPERTY(EditAnywhere, Category = UI)
	UTexture2D* MeleeImage;

	UPROPERTY(EditAnywhere, Category = UI)
	UTexture2D* RangedImage;

	UPROPERTY(EditAnywhere, Category = UI)
	UTexture2D* HealerImage;

	UPROPERTY(EditAnywhere, Category = UI)
	TSubclassOf<UMMOAdvancedFormationButtonWidget> AdvancedButtonClass;

	UFUNCTION(BlueprintCallable, Category = Formation)
	void CreateOrDestroyButtonWidget(TArray<UMMOAdvancedFormationButtonWidget*>& AdvButtonsRef, UTexture2D* ButtonImage);

	UFUNCTION(BlueprintCallable, Category = Formation)
	void DeleteDraggableWidget(TArray<UMMOAdvancedFormationButtonWidget*>& AdvButtonsRef);

	UFUNCTION(BlueprintCallable, Category = Formation)
	void CreateDraggableWidget(TArray<UMMOAdvancedFormationButtonWidget*>& AdvButtonsRef, UTexture2D* ButtonImage);

	UPROPERTY(BlueprintReadWrite, Category = UI)
	TArray<UMMOAdvancedFormationButtonWidget*> TankButtons;

	UPROPERTY(BlueprintReadWrite, Category = UI)
	TArray<UMMOAdvancedFormationButtonWidget*> MeleeButtons;

	UPROPERTY(BlueprintReadWrite, Category = UI)
	TArray<UMMOAdvancedFormationButtonWidget*> RangedButtons;

	UPROPERTY(BlueprintReadWrite, Category = UI)
	TArray<UMMOAdvancedFormationButtonWidget*> HealerButtons;

private:
	UPROPERTY()
	UMMOFormationManager* FormationManager = nullptr;

	UPROPERTY()
	UMMOAdvancedFormationButtonWidget* CurrentPressedButton = nullptr;

	UFUNCTION()
	void OnAdvButtonPressed(UMMOAdvancedFormationButtonWidget* Sender);

	UFUNCTION()
	void OnAdvButtonReleased(UMMOAdvancedFormationButtonWidget* Sender);

	UFUNCTION()
	void TankButtonClicked() { CreateOrDestroyButtonWidget(TankButtons, TankImage); }

	UFUNCTION()
	void MeleeButtonClicked() { CreateOrDestroyButtonWidget(MeleeButtons, MeleeImage); }

	UFUNCTION()
	void RangedButtonClicked() { CreateOrDestroyButtonWidget(RangedButtons, RangedImage); }

	UFUNCTION()
	void HealerButtonClicked() { CreateOrDestroyButtonWidget(HealerButtons, HealerImage); }

	FMMOFormationSetup ComputeFormation();
	TArray<FVector2D> ComputeRescalePointInWindow(const TArray<UMMOAdvancedFormationButtonWidget*>& AdvButtons) const;

	FVector2D ClampToWindow(FVector2D Point, FVector2D Size) const;

	FVector2D ClickOffset = FVector2D::ZeroVector;
};
