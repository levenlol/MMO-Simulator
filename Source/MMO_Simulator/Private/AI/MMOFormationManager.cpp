// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MMOFormationManager.h"
#include "Characters/MMOBaseHero.h"
#include "NavigationSystem.h"
#include "Utils/MMOGameplayUtils.h"

UMMOFormationManager::UMMOFormationManager()
	: Super()
{
	PrimaryComponentTick.bCanEverTick = true;

	StandardSetup.GroupsPositionOffset.Add(EMMOCharacterRole::Tank, FVector2D(0.f, 0.f));
	StandardSetup.GroupsPositionOffset.Add(EMMOCharacterRole::Melee, FVector2D(0.f, 0.33f));
	StandardSetup.GroupsPositionOffset.Add(EMMOCharacterRole::Healer, FVector2D(0.f, 0.66f));
	StandardSetup.GroupsPositionOffset.Add(EMMOCharacterRole::Ranged, FVector2D(0.f, 1.f));
}

// Called when the game starts
void UMMOFormationManager::BeginPlay()
{
	Super::BeginPlay();

	AddPrecachePreviewActors(InitialPreviewsCacheSize);
}

TArray<FVector> UMMOFormationManager::ComputeSimpleFormation(const int32 HeroesNum, const FVector& AnchorPoint, FVector LastPoint, bool bShowPreview)
{
	return ComputeSimpleFormation_Internal(HeroesNum, AnchorPoint, LastPoint, bShowPreview);
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

TArray<FVector> UMMOFormationManager::SortPoints_Nearest(TArray<AMMOBaseHero*> Heroes, TArray<FVector> Points, const FVector& AnchorPoint, const FVector& LastPoint) const
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

TArray<FVector> UMMOFormationManager::SortPoints_Similiar(TArray<AMMOBaseHero*> Heroes, TArray<FVector> Points, const FVector& AnchorPoint, const FVector& LastPoint) const
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

TArray<FVector> UMMOFormationManager::ComputeSimpleFormation_Internal(const int32 CharactersNum, const FVector& AnchorPoint, const FVector& LastPoint, bool bShowPreview /* = true */)
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

TArray<FVector> UMMOFormationManager::ComputeAdvancedFormation_Internal(const FMMOFormationSetup& Setup, const TArray<AMMOBaseHero*>& Heroes, const FVector& AnchorPoint, const FVector& LastPoint, bool bShowPreview)
{
	TArray<AMMOBaseHero*> Tanks = UMMOGameplayUtils::FilterByRole(Heroes, EMMOCharacterRole::Tank);
	TArray<AMMOBaseHero*> Melee = UMMOGameplayUtils::FilterByRole(Heroes, EMMOCharacterRole::Melee);
	TArray<AMMOBaseHero*> Ranged = UMMOGameplayUtils::FilterByRole(Heroes, EMMOCharacterRole::Ranged);
	TArray<AMMOBaseHero*> Healers = UMMOGameplayUtils::FilterByRole(Heroes, EMMOCharacterRole::Healer);

	const FVector Direction = (LastPoint - AnchorPoint).GetSafeNormal2D() * AdvancedFormationLength; // Value range should be between [0, +1]
	const FVector NormalDirection = FVector(-Direction.Y, Direction.X, Direction.Z) * 0.5f; // Half length because X range is [-1, +1]

	const FVector TankOffset = NormalDirection * Setup.GetOffset(EMMOCharacterRole::Tank).X - Direction * Setup.GetOffset(EMMOCharacterRole::Tank).Y;
	const FVector MeleeOffset = NormalDirection * Setup.GetOffset(EMMOCharacterRole::Melee).X - Direction * Setup.GetOffset(EMMOCharacterRole::Melee).Y;
	const FVector RangedOffset = NormalDirection * Setup.GetOffset(EMMOCharacterRole::Ranged).X - Direction * Setup.GetOffset(EMMOCharacterRole::Ranged).Y;
	const FVector HealerOffset = NormalDirection * Setup.GetOffset(EMMOCharacterRole::Healer).X - Direction * Setup.GetOffset(EMMOCharacterRole::Healer).Y;

	TArray<FVector> TankPoints = ComputeSimpleFormation(Tanks.Num(), AnchorPoint + TankOffset, LastPoint + TankOffset, true);
	TArray<FVector> MeleePoints = ComputeSimpleFormation(Melee.Num(), AnchorPoint + MeleeOffset, LastPoint + MeleeOffset, true);
	TArray<FVector> RangedPoints = ComputeSimpleFormation(Ranged.Num(), AnchorPoint + RangedOffset, LastPoint + RangedOffset, true);
	TArray<FVector> HealerPoints = ComputeSimpleFormation(Healers.Num(), AnchorPoint + HealerOffset, LastPoint + HealerOffset, true);

	TankPoints = SortPoints(Tanks, TankPoints, AnchorPoint + TankOffset, LastPoint + TankOffset);
	MeleePoints = SortPoints(Melee, MeleePoints, AnchorPoint + MeleeOffset, LastPoint + MeleeOffset);
	RangedPoints = SortPoints(Ranged, RangedPoints, AnchorPoint + RangedOffset, LastPoint + RangedOffset);
	HealerPoints = SortPoints(Healers, HealerPoints, AnchorPoint + HealerOffset, LastPoint + HealerOffset);


	int32 TankIdx = 0;
	int32 HealerIdx = 0;
	int32 RangedIdx = 0;
	int32 MeleeIdx = 0;


	TArray<FVector> Points;
	for (AMMOBaseHero* Hero : Heroes)
	{
		if (!Hero)
			continue;

		if (Hero->CharacterInfo.CharacterRole == EMMOCharacterRole::Tank)
		{
			Points.Add(TankPoints[TankIdx]);
			TankIdx++;
		}
		else if (Hero->CharacterInfo.CharacterRole == EMMOCharacterRole::Melee)
		{
			Points.Add(MeleePoints[MeleeIdx]);
			MeleeIdx++;
		}
		else if (Hero->CharacterInfo.CharacterRole == EMMOCharacterRole::Ranged)
		{
			Points.Add(RangedPoints[RangedIdx]);
			RangedIdx++;
		}
		else if (Hero->CharacterInfo.CharacterRole == EMMOCharacterRole::Healer)
		{
			Points.Add(HealerPoints[HealerIdx]);
			HealerIdx++;
		}
	}

	return Points;
}

TArray<FTransform> UMMOFormationManager::ComputeFormation(const TArray<AMMOBaseHero*>& Heroes, const FVector& AnchorPoint, FVector LastPoint, bool bShowPreview)
{
	TArray<FVector> Points;

	if (FMath::IsNearlyZero((AnchorPoint - LastPoint).SizeSquared(), 0.1f))
	{
		// Better approximation than using FVector::UpVector
		const FVector MiddlePoint = UMMOGameplayUtils::ComputeHeroesSelectionMiddlePoint(Heroes);
		LastPoint = AnchorPoint + ((AnchorPoint - MiddlePoint).GetSafeNormal() * 100.f);
	}

	if (FormationType == EMMOFormationType::Simple)
	{
		// Standard Quad Formation
		Points = ComputeSimpleFormation(Heroes.Num(), AnchorPoint, LastPoint, bShowPreview);
		Points = SortPoints(Heroes, Points, AnchorPoint, LastPoint);
	}
	else if (FormationType == EMMOFormationType::Standard)
	{
		Points = ComputeAdvancedFormation_Internal(StandardSetup, Heroes, AnchorPoint, LastPoint, bShowPreview);
	}
	else if (FormationType == EMMOFormationType::Advanced)
	{
		Points = ComputeAdvancedFormation_Internal(HasValidAdvancedFormation() ? AdvancedSetupFormation : StandardSetup, Heroes, AnchorPoint, LastPoint, bShowPreview);
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

	check(Points.Num() == Heroes.Num());

	const FRotator Rotation = (LastPoint - AnchorPoint).GetSafeNormal().ToOrientationRotator();
	TArray<FTransform> Transforms;
	Algo::Transform(Points, Transforms, [&Rotation](const FVector& Point) { return FTransform(Rotation, Point, FVector::OneVector); });

	return Transforms;
}

TArray<FVector> UMMOFormationManager::SortPoints(const TArray<AMMOBaseHero*>& Heroes, const TArray<FVector>& Points, const FVector& AnchorPoint, const FVector& LastPoint) const
{
	if (FormationSortType == EMMOFormationSortType::Nearest)
	{
		return SortPoints_Nearest(Heroes, Points, AnchorPoint, LastPoint);
	}
	else if (FormationSortType == EMMOFormationSortType::Similar)
	{
		return SortPoints_Similiar(Heroes, Points, AnchorPoint, LastPoint);
	}

	return Points;
}

bool UMMOFormationManager::HasValidAdvancedFormation() const
{
	return AdvancedSetupFormation.GroupsPositionOffset.Num() == static_cast<int32>(EMMOCharacterRole::MAX);
}

void UMMOFormationManager::SetAdvancedFormation(const FMMOFormationSetup& InSetup)
{
	AdvancedSetupFormation = InSetup;
}

FVector2D FMMOFormationSetup::GetOffset(EMMOCharacterRole CharacterRole) const
{
	if (GroupsPositionOffset.Contains(CharacterRole))
	{
		const FVector2D Offset = GroupsPositionOffset[CharacterRole];
		const float X = FMath::Clamp(Offset.X, -1.f, 1.f);
		const float Y = FMath::Clamp(Offset.Y, -1.f, 1.f);

		return FVector2D(X, Y);
	}

	check(0);
	return FVector2D::ZeroVector;
}
