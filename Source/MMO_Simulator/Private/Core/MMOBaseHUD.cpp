// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MMOBaseHUD.h"
#include "Characters/MMOBaseHero.h"

AMMOBaseHUD::AMMOBaseHUD()
	: Super()
{

}

void AMMOBaseHUD::DrawHUD()
{
	Super::DrawHUD();

	AMMOPlayerController* PC = GetOwningPlayerController();

	if (!PC)
	{
		return;
	}

	if (PC->IsSelecting() && SelectionTexture)
	{
		const FVector2D& Start = PC->GetMouseClickLocation();
		const FVector2D& End = PC->GetCurrentMousePosition();

		FLinearColor Color(1.f, 1.f, 1.f, TextureAlpha);
		DrawTexture(SelectionTexture, End.X, End.Y, Start.X - End.X, Start.Y - End.Y, 0.f, 0.f, 0.f, 0.f, Color);

		TArray<AMMOBaseHero*> SelectedHeroes;

		GetActorsInSelectionRectangle<AMMOBaseHero>(Start, End, SelectedHeroes);

		PC->SetSelectedHeroes(SelectedHeroes);
	}
}
