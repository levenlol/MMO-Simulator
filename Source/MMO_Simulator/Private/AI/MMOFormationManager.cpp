// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MMOFormationManager.h"
#include "Characters/MMOBaseHero.h"

#pragma optimize("", off)

UMMOFormationManager::UMMOFormationManager()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UMMOFormationManager::BeginPlay()
{
	Super::BeginPlay();

}


// Called every frame
void UMMOFormationManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

TArray<FVector> UMMOFormationManager::ComputeFormation(const TArray<AMMOBaseHero*>& InHeroes, const FVector& AnchorPoint, const FVector& LastPoint)
{
	const int32 HeroesNum = InHeroes.Num();

	TArray<FVector> Points;
	Points.Reserve(HeroesNum);
	
	const float InitialLateralOffset = HeroesNum & 0x1 ? 0.f : HorizontalMargin / 2.f;
	const int32 InitialLateralNum = (HeroesNum - 1)/ 2;
	const int32 ColumnsNum = (HeroesNum / MaxHeroesPerRow) + 1;

	FVector Tangent = FMath::IsNearlyZero((LastPoint - AnchorPoint).SizeSquared()) ? FVector::ForwardVector : (LastPoint - AnchorPoint);
	Tangent.Z = 0.f;
	Tangent.Normalize();

	const FVector Side(-Tangent.Y, Tangent.X, 0.f);

	for (int32 j = 0; j < HeroesNum; j++)
	{
		const int32 ColumnNum = j % MaxHeroesPerRow;
		const int32 RowNum = j / MaxHeroesPerRow;


		const FVector Point = AnchorPoint + ((ColumnNum - InitialLateralNum) * Side * HorizontalMargin) + (Tangent * RowNum * VerticalMargin) - Side * InitialLateralOffset; // anchor + lateral + vertical - offset
		Points.Add(Point);

		DrawDebugSphere(GetWorld(), Point, 32.f, 8, FColor::Blue, false, -1.f, 0, 5.f);
	}
	
	return Points;
}

