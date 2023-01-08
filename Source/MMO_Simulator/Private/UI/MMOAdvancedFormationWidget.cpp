// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MMOAdvancedFormationWidget.h"
#include "AI/MMOFormationManager.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Utils/MMOGameplayUtils.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UMMOAdvancedFormationButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Button->OnPressed.AddDynamic(this, &UMMOAdvancedFormationButtonWidget::OnButtonPressed);
	Button->OnReleased.AddDynamic(this, &UMMOAdvancedFormationButtonWidget::OnButtonReleased);
}

void UMMOAdvancedFormationButtonWidget::NativeDestruct()
{
	Super::NativeDestruct();

	Button->OnPressed.RemoveDynamic(this, &UMMOAdvancedFormationButtonWidget::OnButtonPressed);
	Button->OnReleased.RemoveDynamic(this, &UMMOAdvancedFormationButtonWidget::OnButtonReleased);
}

void UMMOAdvancedFormationWidget::NativeConstruct()
{
	Super::NativeConstruct();

	FormationManager = GetOwningPlayer()->FindComponentByClass<UMMOFormationManager>();

	TankButton->OnClicked.AddDynamic(this, &UMMOAdvancedFormationWidget::TankButtonClicked);
	MeleeButton->OnClicked.AddDynamic(this, &UMMOAdvancedFormationWidget::MeleeButtonClicked);
	RangedButton->OnClicked.AddDynamic(this, &UMMOAdvancedFormationWidget::RangedButtonClicked);
	HealerButton->OnClicked.AddDynamic(this, &UMMOAdvancedFormationWidget::HealerButtonClicked);
}

void UMMOAdvancedFormationWidget::NativeDestruct()
{
	Super::NativeDestruct();

	FormationManager = nullptr;

	TankButton->OnClicked.RemoveDynamic(this, &UMMOAdvancedFormationWidget::TankButtonClicked);
	MeleeButton->OnClicked.RemoveDynamic(this, &UMMOAdvancedFormationWidget::MeleeButtonClicked);
	RangedButton->OnClicked.RemoveDynamic(this, &UMMOAdvancedFormationWidget::RangedButtonClicked);
	HealerButton->OnClicked.RemoveDynamic(this, &UMMOAdvancedFormationWidget::HealerButtonClicked);
}

void UMMOAdvancedFormationWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (CurrentPressedButton)
	{
		FVector2D FinalPosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(this) + ClickOffset;
		FinalPosition = ClampToWindow(FinalPosition, CurrentPressedButton->Button->GetCachedGeometry().GetLocalSize());

		CurrentPressedButton->SetRenderTranslation(FinalPosition);
	}
}

void UMMOAdvancedFormationWidget::CreateOrDestroyButtonWidget(TArray<UMMOAdvancedFormationButtonWidget*>& AdvButtonsRef, UTexture2D* ButtonImage)
{
	if (GetOwningPlayer()->IsInputKeyDown(EKeys::LeftShift))
	{
		DeleteDraggableWidget(AdvButtonsRef);
	}
	else
	{
		CreateDraggableWidget(AdvButtonsRef, ButtonImage);
	}
}

void UMMOAdvancedFormationWidget::DeleteDraggableWidget(TArray<UMMOAdvancedFormationButtonWidget*>& AdvButtonsRef)
{
	const int32 ButtonsNum = AdvButtonsRef.Num();
	if (ButtonsNum > 0)
	{
		UMMOAdvancedFormationButtonWidget* Button = AdvButtonsRef[ButtonsNum - 1];
		Button->OnPressed().RemoveAll(this);
		Button->OnReleased().RemoveAll(this);

		MapCanvas->RemoveChild(Button);
		Button->RemoveFromParent();
		AdvButtonsRef.RemoveAt(ButtonsNum - 1);
	}
}

void UMMOAdvancedFormationWidget::CreateDraggableWidget(TArray<UMMOAdvancedFormationButtonWidget*>& AdvButtonsRef, UTexture2D* ButtonImage)
{
	check(AdvancedButtonClass);
	if (AdvancedButtonClass)
	{
		UMMOAdvancedFormationButtonWidget* Button = CreateWidget<UMMOAdvancedFormationButtonWidget>(GetOwningPlayer(), AdvancedButtonClass);
		UMMOGameplayUtils::SetImageToButton(Button->Button, ButtonImage);

		MapCanvas->AddChild(Button);

		Button->OnPressed().AddUObject(this, &UMMOAdvancedFormationWidget::OnAdvButtonPressed);
		Button->OnReleased().AddUObject(this, &UMMOAdvancedFormationWidget::OnAdvButtonReleased);

		AdvButtonsRef.Add(Button);

		// Draw it to the center of the canvas
		const FVector2D Size = MapCanvas->GetCachedGeometry().GetLocalSize();

		const FVector2D Position = MapCanvas->GetRenderTransform().Translation + Size * 0.5f;
		Button->SetRenderTranslation(Position - FVector2D(32.f));
	}
}

void UMMOAdvancedFormationWidget::OnAdvButtonPressed(UMMOAdvancedFormationButtonWidget* Sender)
{
	CurrentPressedButton = Sender;

	const FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
	ClickOffset = CurrentPressedButton->GetRenderTransform().Translation - MousePosition;
}

void UMMOAdvancedFormationWidget::OnAdvButtonReleased(UMMOAdvancedFormationButtonWidget* Sender)
{
	if (Sender == CurrentPressedButton)
	{
		CurrentPressedButton = nullptr;
		FormationManager->SetAdvancedFormation(ComputeFormation());
	}
}

FMMOFormationSetup UMMOAdvancedFormationWidget::ComputeFormation()
{
	FMMOFormationSetup Setup;
	
	Setup.Add(EMMOCharacterRole::Tank, ComputeRescalePointInWindow(TankButtons));
	Setup.Add(EMMOCharacterRole::Melee, ComputeRescalePointInWindow(MeleeButtons));
	Setup.Add(EMMOCharacterRole::Ranged, ComputeRescalePointInWindow(RangedButtons));
	Setup.Add(EMMOCharacterRole::Healer, ComputeRescalePointInWindow(HealerButtons));

	return Setup;
}

TArray<FVector2D> UMMOAdvancedFormationWidget::ComputeRescalePointInWindow(const TArray<UMMOAdvancedFormationButtonWidget*>& AdvButtons) const
{
	TArray<FVector2D> Points;
	const FVector2D MapSize = MapCanvas->GetCachedGeometry().GetLocalSize();

	for (int32 i = 0; i < AdvButtons.Num(); i++)
	{
		const FVector2D ButtonSize = AdvButtons[i]->GetCachedGeometry().GetLocalSize();
		const FVector2D ButtonPosition = AdvButtons[i]->GetRenderTransform().Translation;

		// We need to remap the value because the button isnt 0 sized and it is clamped inside the view. If view size is 512 and button size is 64 value range is 0 - 448
		const float InX = MapSize.X - ButtonSize.X * 0.5f;
		float X = FMath::GetMappedRangeValueClamped(FVector2D(0.f, InX), FVector2D(0.f, MapSize.X), ButtonPosition.X);

		const float InY = MapSize.Y - ButtonSize.Y * 0.5f;
		float Y = FMath::GetMappedRangeValueClamped(FVector2D(0.f, InY), FVector2D(0.f, MapSize.Y), ButtonPosition.Y);

		// Out range for X should be between -0.5f and 0.5f
		static const FVector2D XOutRange(-0.5f, 0.5f);
		X = FMath::GetMappedRangeValueClamped(FVector2D(0.f, MapSize.X), XOutRange, X);

		// Out range for X should be between 0.f and 1.f
		static const FVector2D YOutRange(0.f, 1.f);
		Y = FMath::GetMappedRangeValueClamped(FVector2D(0.f, MapSize.Y), YOutRange, Y);

		Points.Add(FVector2D(X, Y));
	}

	return Points;
}

FVector2D UMMOAdvancedFormationWidget::ClampToWindow(FVector2D Point, FVector2D Size) const
{
	const FVector2D MapSize = MapCanvas->GetCachedGeometry().GetLocalSize();
	const FVector2D MapPosition = MapCanvas->GetRenderTransform().Translation;

	float X = FMath::Max(Point.X, MapPosition.X);
	float Y = FMath::Max(Point.Y, MapPosition.Y);

	const float MaxX = MapSize.X + MapPosition.X - Size.X;
	const float MaxY = MapSize.Y + MapPosition.Y - Size.Y;

	X = FMath::Min(X, MaxX);
	Y = FMath::Min(Y, MaxY);

	return FVector2D(X, Y);
}
