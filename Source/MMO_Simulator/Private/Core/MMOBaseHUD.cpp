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
		GetActorsInSelectionRectangle<AMMOBaseHero>(Start, End, SelectedHeroes, false);

		if ((End - Start).Size() <= 5.f && SelectedHeroes.Num() > 0)
		{
			// we only want to select the nearest Hero
			FVector WorldLocation, Direction;
			if (PC->HasValidMousePositions())
			{
				const FVector MousePosition(PC->GetCurrentMousePosition(), 0.f);
				AMMOBaseHero* SelectedHero = SelectedHeroes[0];
				float MinDistance = (MousePosition - Project(SelectedHero->GetActorLocation(), true)).Size2D();

				// find nearest
				for (int32 i = 1; i < SelectedHeroes.Num(); i++)
				{
					const float Dist = (MousePosition - Project(SelectedHeroes[i]->GetActorLocation(), true)).Size2D();
					if (Dist < MinDistance)
					{
						SelectedHero = SelectedHeroes[i];
						MinDistance = Dist;
					}
				}

				PC->SetSelectedHeroes({ SelectedHero });
			}
		}
		else
		{
			PC->SetSelectedHeroes(SelectedHeroes);
		}
	}
}
