// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MovableWidget.h"
#include "Components/Button.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UMovableWidget::NativeConstruct()
{
	Super::NativeConstruct();

	bMoving = false;
	TopWindowBar->OnPressed.AddDynamic(this, &UMovableWidget::TopWindowBarPressed);
	TopWindowBar->OnReleased.AddDynamic(this, &UMovableWidget::TopWindowBarReleased);
	CloseWindowButton->OnClicked.AddDynamic(this, &UMovableWidget::CloseClicked);
}

void UMovableWidget::NativeDestruct()
{
	Super::NativeDestruct();

	TopWindowBar->OnPressed.RemoveDynamic(this, &UMovableWidget::TopWindowBarPressed);
	TopWindowBar->OnReleased.RemoveDynamic(this, &UMovableWidget::TopWindowBarReleased);
	CloseWindowButton->OnClicked.RemoveDynamic(this, &UMovableWidget::CloseClicked);
}

void UMovableWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bMoving)
	{
		FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
		GetRootWidget()->SetRenderTranslation(MousePosition - ClickOffset);
	}
}

void UMovableWidget::TopWindowBarPressed()
{
	bMoving = true;

	const FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
	const FVector2D WidgetPos = GetRootWidget()->RenderTransform.Translation;
	ClickOffset = MousePosition - WidgetPos;
}

void UMovableWidget::TopWindowBarReleased()
{
	bMoving = false;
}

void UMovableWidget::CloseClicked()
{
	RemoveFromViewport();
}
