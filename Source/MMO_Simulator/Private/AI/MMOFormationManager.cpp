// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MMOFormationManager.h"
#include "Characters/MMOBaseHero.h"
#include "NavigationSystem.h"
#include "Utils/MMOGameplayUtils.h"

UMMOFormationManager::UMMOFormationManager()
	: Super()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UMMOFormationManager::BeginPlay()
{
	Super::BeginPlay();

	AddPrecachePreviewActors(InitialPreviewsCacheSize);
}

void UMMOFormationManager::AddPrecachePreviewActors(const int32 PreviewNumbersToAdd)
{
	if (!UIFormationPreviewClass)
	{
		UE_LOG(LogTemp, Error, TEXT("Dont have a valid UIFormationPreviewClass. Please set it."));
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.bNoFail = true;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for (int32 i = 0; i < PreviewNumbersToAdd; i++)
	{
		AActor* NewActor = GetWorld()->SpawnActor<AActor>(UIFormationPreviewClass, SpawnParams);
		NewActor->SetActorEnableCollision(false);
		NewActor->SetActorHiddenInGame(true);

		UIFormationPreviews.Add(NewActor);
	}

	ensure(UIFormationPreviews.Num() < 60);
}

FVector UMMOFormationManager::ProjectPointToNavMesh(const FVector& InLocation) const
{
	UNavigationSystemV1* NavigationSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavigationSystem)
	{
		UE_LOG(LogTemp, Error, TEXT("FormationManager: cannot find UNavigationSystemV1"));
		return InLocation;
	}

	FNavLocation EndLocation;
	if (NavigationSystem->ProjectPointToNavigation(InLocation, EndLocation, FVector(500.f)))
	{
		return EndLocation.Location;
	}

	return InLocation;
}

TArray<FVector> UMMOFormationManager::SortPoints_Nearest(TArray<AMMOBaseHero*> Heroes, TArray<FVector> Points, const FVector& AnchorPoint, const FVector& LastPoint)
{
	TArray<FVector> SortedPoints;
	SortedPoints.Init(FVector::ZeroVector, Points.Num());

	int32 HeroIdx = -1;
	int32 PointIdx = -1;

	float MinDist = BIG_NUMBER;
	const int32 HeroesNum = Heroes.Num();
	for (int32 k = 0; k < HeroesNum; k++)
	{
		HeroIdx = -1;
		PointIdx = -1;

		MinDist = BIG_NUMBER;

		for (int32 i = 0; i < Heroes.Num(); i++)
		{
			AMMOBaseHero* Hero = Heroes[i];
			if (!Hero)
				continue;

			for (int j = 0; j < Points.Num(); j++)
			{
				const FVector& Point = Points[j];

				const float CurrDist = (Hero->GetActorLocation() - Point).SizeSquared();
				if (CurrDist < MinDist)
				{
					MinDist = CurrDist;
					HeroIdx = i;
					PointIdx = j;
				}
			}
		}


		SortedPoints[HeroIdx] = Points[PointIdx];
		Points[PointIdx] = FVector(1.84e+18f); // this point shouldnt be selected anymore.
		Heroes[HeroIdx] = nullptr; // this hero has been already addressed.
	}

	return SortedPoints;
}

TArray<FVector> UMMOFormationManager::SortPoints_Similiar(TArray<AMMOBaseHero*> Heroes, TArray<FVector> Points, const FVector& AnchorPoint, const FVector& LastPoint)
{
	struct DummyContainer
	{
		int32 Idx;
		float Value;
	};

	const FVector MiddlePoint = UMMOGameplayUtils::ComputeHeroesSelectionMiddlePoint(Heroes);

	if (MiddlePoint == AnchorPoint)
		return Points;


	const FVector Direction = (MiddlePoint - AnchorPoint).GetSafeNormal2D();
	const FVector NormalDirection(-Direction.Y, Direction.X, Direction.Z);

	TArray<FVector> SortedPoints;
	SortedPoints.Init(FVector::ZeroVector, Points.Num());

	TArray<DummyContainer> PrecomputedPointsDot;

	for (int32 i = 0; i < Points.Num(); i++)
	{
		const FVector DirToPoint = (Points[i] - MiddlePoint);
		const float Value = DirToPoint | NormalDirection;

		DummyContainer D;
		D.Idx = i;
		D.Value = Value;

		PrecomputedPointsDot.Add(D);
	}

	TArray<DummyContainer> PrecomputedHeroesDot;
	for (int32 i = 0; i < Heroes.Num(); i++)
	{
		const FVector DirToHero = (Heroes[i]->GetActorLocation() - AnchorPoint);
		const float Value = DirToHero | NormalDirection;

		DummyContainer D;
		D.Idx = i;
		D.Value = Value;

		PrecomputedHeroesDot.Add(D);
	}

	PrecomputedPointsDot.Sort([](const DummyContainer& A, const DummyContainer& B) { return A.Value < B.Value; });
	PrecomputedHeroesDot.Sort([](const DummyContainer& A, const DummyContainer& B) { return A.Value < B.Value; });

	for (int32 i = 0; i < Heroes.Num(); i++)
	{
		SortedPoints[PrecomputedHeroesDot[i].Idx] = Points[PrecomputedPointsDot[i].Idx];
	}

#if WITH_EDITORONLY_DATA
	if (bDebug)
	{
		DrawDebugLine(GetWorld(), AnchorPoint + FVector::UpVector * 100.f, MiddlePoint + FVector::UpVector * 100.f, FColor::Magenta, false, -1.f, 1, 5.f);
		
		DrawDebugSphere(GetWorld(), MiddlePoint + FVector::UpVector * 100.f, 16.f, 8, FColor::Magenta, false, -1.f, 0, 3.f);
		DrawDebugSphere(GetWorld(), AnchorPoint + FVector::UpVector * 100.f, 16.f, 8, FColor::Magenta, false, -1.f, 0, 3.f);


		FlushDebugStrings(GetWorld());
		for (int32 i = 0; i < PrecomputedHeroesDot.Num(); i++)
		{
			DrawDebugString(GetWorld(), Heroes[PrecomputedHeroesDot[i].Idx]->GetActorLocation(), FString::SanitizeFloat(PrecomputedHeroesDot[i].Value));
			DrawDebugString(GetWorld(), Points[PrecomputedPointsDot[i].Idx], FString::SanitizeFloat(PrecomputedPointsDot[i].Value));
		}
	}
#endif

	return SortedPoints;
}

void UMMOFormationManager::ShowPreview(const TArray<FVector>& Points)
{
	for (const FVector& Point : Points)
	{
		ShowPreview(Point);
	}
}

void UMMOFormationManager::ShowPreview(FVector Point)
{
	if (++CurrentPreviewIndex >= UIFormationPreviews.Num())
	{
		AddPrecachePreviewActors(1);
	}

	UIFormationPreviews[CurrentPreviewIndex]->SetActorHiddenInGame(false);
	UIFormationPreviews[CurrentPreviewIndex]->SetActorLocation(Point);
}

void UMMOFormationManager::HideAllPreviews()
{
	for (int32 i = 0; i <= CurrentPreviewIndex; i++)
	{
		UIFormationPreviews[i]->SetActorHiddenInGame(true);
	}

	CurrentPreviewIndex = -1;
}


// Called every frame
void UMMOFormationManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

TArray<FVector> UMMOFormationManager::ComputeSimpleFormation(const int32 CharactersNum, const FVector& AnchorPoint, const FVector& LastPoint, bool bShowPreview /* = true */)
{
	if (CharactersNum <= 0)
		return {};

	TArray<FVector> Points;
	Points.Reserve(CharactersNum);
	
	const int32 SafeCharacterRowNum = FMath::Min(MaxHeroesPerRow, CharactersNum);
	const float InitialLateralOffset = SafeCharacterRowNum & 0x1 ? 0.f : HorizontalMargin / 2.f;
	const int32 InitialLateralNum = (SafeCharacterRowNum - 1)/ 2;
	const int32 ColumnsNum = (SafeCharacterRowNum / MaxHeroesPerRow) + 1;

	FVector Tangent = FMath::IsNearlyZero((LastPoint - AnchorPoint).SizeSquared()) ? FVector::ForwardVector : (LastPoint - AnchorPoint);
	Tangent.Z = 0.f;
	Tangent.Normalize();

	const FVector Side(-Tangent.Y, Tangent.X, 0.f);

	for (int32 j = 0; j < CharactersNum; j++)
	{
		const int32 ColumnNum = j % MaxHeroesPerRow;
		const int32 RowNum = j / MaxHeroesPerRow;

		FVector Point = AnchorPoint + ((ColumnNum - InitialLateralNum) * Side * HorizontalMargin) - (Tangent * RowNum * VerticalMargin) - Side * InitialLateralOffset; // anchor + lateral + vertical - offset
		Point = ProjectPointToNavMesh(Point);
		Points.Add(Point);

		if (bShowPreview)
		{
			ShowPreview(Point);
		}
	}

	return Points;
}

TArray<FTransform> UMMOFormationManager::ComputeFormation(const TArray<AMMOBaseHero*>& Heroes, const FVector& AnchorPoint, FVector LastPoint, bool bShowPreview)
{
	if (FMath::IsNearlyZero((AnchorPoint - LastPoint).SizeSquared(), 0.1f))
	{
		// Better approximation than using FVector::UpVector
		const FVector MiddlePoint = UMMOGameplayUtils::ComputeHeroesSelectionMiddlePoint(Heroes);
		LastPoint = AnchorPoint + ((AnchorPoint - MiddlePoint).GetSafeNormal() * 100.f);
	}

	TArray<FVector> Points = ComputeSimpleFormation(Heroes.Num(), AnchorPoint, LastPoint, bShowPreview);

	if (FormationSortType == EMMOFormationSortType::Nearest)
	{
		Points = SortPoints_Nearest(Heroes, Points, AnchorPoint, LastPoint);
	}
	else if (FormationSortType == EMMOFormationSortType::Similar)
	{
		Points = SortPoints_Similiar(Heroes, Points, AnchorPoint, LastPoint);
	}

#if WITH_EDITORONLY_DATA
	if (bDebug)
	{
		for (int32 i = 0; i < Points.Num(); i++)
		{
			DrawDebugLine(GetWorld(), Heroes[i]->GetActorLocation() + FVector::UpVector * 100.f, Points[i], FColor::Blue, false, -1.f, 1, 3.f);
		}
	}
#endif

	const FRotator Rotation = (LastPoint - AnchorPoint).GetSafeNormal().ToOrientationRotator();
	TArray<FTransform> Transforms;
	Algo::Transform(Points, Transforms, [&Rotation](const FVector& Point) { return FTransform(Rotation, Point, FVector::OneVector); });

	return Transforms;
}

