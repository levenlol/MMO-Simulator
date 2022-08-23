// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MMOFormationManager.h"
#include "Characters/MMOBaseHero.h"
#include "NavigationSystem.h"
#include "Utils/MMOGameplayUtils.h"

UMMOFormationManager::UMMOFormationManager()
	: Super()
{
	PrimaryComponentTick.bCanEverTick = true;

	StandardSetup.Add(EMMOCharacterRole::Tank, { FVector2D(0.f, 0.f) });
	StandardSetup.Add(EMMOCharacterRole::Melee, { FVector2D(0.f, 0.33f) });
	StandardSetup.Add(EMMOCharacterRole::Healer, { FVector2D(0.f, 0.66f) });
	StandardSetup.Add(EMMOCharacterRole::Ranged, { FVector2D(0.f, 1.f) });
}

// Called when the game starts
void UMMOFormationManager::BeginPlay()
{
	Super::BeginPlay();

	AddPrecachePreviewActors(InitialPreviewsCacheSize);
}

TArray<FVector> UMMOFormationManager::ComputeSimpleFormation(const int32 HeroesNum, const FVector& AnchorPoint, FVector LastPoint, bool bShowPreview)
{
	TArray<FVector> Points = ComputeSimpleFormation_Internal(SimpleFormationTuning, HeroesNum, AnchorPoint, LastPoint);

	if (bShowPreview)
	{
		ShowPreview(Points);
	}

	return Points;
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

TArray<FVector> UMMOFormationManager::SortPoints_Nearest(TArray<AMMOBaseHero*> Heroes, TArray<FVector> Points, const FVector& AnchorPoint) const
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

TArray<FVector> UMMOFormationManager::SortPoints_Similiar(TArray<AMMOBaseHero*> Heroes, TArray<FVector> Points, const FVector& AnchorPoint) const
{
	struct DummyContainer
	{
		int32 Idx;
		float Value;
	};

	if (Heroes.Num() * Points.Num() == 0)
		return Points;

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

/*
Points 5
Container 3

[2,2,1]

i = 0,1,2

j1=0,1
j2=0,1
j3=0

in = i + j * Container
i1 = 0 + 0 * 3 = 0
i2 = 0 + 1 * 3 = 3
i3 = 1 + 0 * 3 = 1
i4 = 1 + 1 * 3 = 4
i5 = 2 + 0 * 3 = 2
*/
TArray<FVector> UMMOFormationManager::ComputeSimpleFormation_Internal(const FMMOFormationTuning& FormationTuning, const int32 CharactersNum, const TArray<FMMOFormationPoint>& AnchorPoints) const
{
	if (CharactersNum * AnchorPoints.Num() <= 0)
	{
		return {};
	}

	TArray<TArray<FVector>> PointsContainer;
	TArray<FVector> FlattenPoints;
	FlattenPoints.SetNum(CharactersNum);

	// Create various simple formation. we use Division results and reminder to calculate how many characters fit in each formation.
	const int32 Div = CharactersNum / AnchorPoints.Num();
	int32 Rem = CharactersNum % AnchorPoints.Num();

	int32 GeneratedPoints = 0;
	int32 Index = 0;
	while (GeneratedPoints < CharactersNum)
	{
		const FMMOFormationPoint& AnchorPoint = AnchorPoints[Index];

		TArray<FVector> Points = ComputeSimpleFormation_Internal(FormationTuning, Div + Rem, AnchorPoint.AnchorPoint, AnchorPoint.LastPoint);
		GeneratedPoints += Points.Num();

		PointsContainer.Add(Points);
		Rem = FMath::Max(Rem - 1, 0);
		Index++;
	}

	check(Rem == 0);

	for (int32 i = 0; i < PointsContainer.Num(); i++)
	{
		const TArray<FVector>& Points = PointsContainer[i];
		for (int32 j = 0; j < Points.Num(); j++)
		{
			FlattenPoints[i + j * PointsContainer.Num()] = Points[j];
		}
	}

	return FlattenPoints;
}

TArray<FVector> UMMOFormationManager::ComputeSimpleFormation_Internal(const FMMOFormationTuning& FormationTuning, const int32 CharactersNum, const FVector& AnchorPoint, const FVector& LastPoint) const
{
	if (CharactersNum <= 0)
		return {};

	TArray<FVector> Points;
	Points.Reserve(CharactersNum);
	
	const float Distance = (LastPoint - AnchorPoint).Size();

	const int32 MaxHeroesPerRow = FormationTuning.GetMaxHeroesPerRow(Distance);
	const float HorizontalMargin = FormationTuning.GetHorizontalMargin(Distance);
	const float VerticalMargin = FormationTuning.GetVerticalMargin(Distance);

	FVector Tangent = FMath::IsNearlyZero((LastPoint - AnchorPoint).SizeSquared()) ? FVector::ForwardVector : (LastPoint - AnchorPoint);
	Tangent.Z = 0.f;
	Tangent.Normalize();

	const FVector Side(-Tangent.Y, Tangent.X, 0.f);

	const int32 TotalRowsNum = ((CharactersNum - 1) / MaxHeroesPerRow) + 1;

	if (FormationPositioningType == EMMOFormationPositioningType::UpDown)
	{
		int32 HeroesToPlace = CharactersNum;
		for (int32 i = 0; i < TotalRowsNum; i++)
		{
			const int32 SafeCharacterRowNum = FMath::Min(MaxHeroesPerRow, HeroesToPlace);
			const float InitialLateralOffset = SafeCharacterRowNum & 0x1 ? 0.f : HorizontalMargin / 2.f;
			const int32 InitialLateralNum = (SafeCharacterRowNum - 1) / 2;
			const int32 ColumnsNum = (SafeCharacterRowNum / MaxHeroesPerRow) + 1;

			for (int j = 0; j < SafeCharacterRowNum; j++)
			{
				const int32 ColumnNum = j % SafeCharacterRowNum;
				const int32 RowNum = i;

				FVector Point = AnchorPoint + ((ColumnNum - InitialLateralNum) * Side * HorizontalMargin) - (Tangent * RowNum * VerticalMargin) - Side * InitialLateralOffset; // anchor + lateral + vertical - offset
				Point = ProjectPointToNavMesh(Point);
				Points.Add(Point);

				HeroesToPlace--;
			}
		}
	}
	else if (FormationPositioningType == EMMOFormationPositioningType::LeftRight)
	{
		int32 HeroesToPlace = CharactersNum;
		for (int32 i = 0; i < TotalRowsNum; i++)
		{
			const int32 SafeCharacterRowNum = FMath::Min(MaxHeroesPerRow, HeroesToPlace);
			const int32 ColumnsNum = (SafeCharacterRowNum / MaxHeroesPerRow) + 1;

			for (int j = 0; j < SafeCharacterRowNum; j++)
			{
				const int32 ColumnNum = j % SafeCharacterRowNum;
				const int32 RowNum = i;

				FVector Point = AnchorPoint + j * Tangent * HorizontalMargin + Side * i * VerticalMargin;
				Point = ProjectPointToNavMesh(Point);
				Points.Add(Point);

				HeroesToPlace--;
			}
		}
	}

	return Points;
}

TArray<FVector> UMMOFormationManager::ComputeAdvancedFormation_Internal(const FMMOFormationTuning& FormationTuning, const FMMOFormationSetup& Setup, const TArray<AMMOBaseHero*>& Heroes, const FVector& AnchorPoint, const FVector& LastPoint)
{
	TArray<AMMOBaseHero*> Tanks = UMMOGameplayUtils::FilterByRole(Heroes, EMMOCharacterRole::Tank);
	TArray<AMMOBaseHero*> Melee = UMMOGameplayUtils::FilterByRole(Heroes, EMMOCharacterRole::Melee);
	TArray<AMMOBaseHero*> Ranged = UMMOGameplayUtils::FilterByRole(Heroes, EMMOCharacterRole::Ranged);
	TArray<AMMOBaseHero*> Healers = UMMOGameplayUtils::FilterByRole(Heroes, EMMOCharacterRole::Healer);

	FVector Direction = (LastPoint - AnchorPoint).GetSafeNormal2D() * AdvancedFormationLength;
	FVector NormalDirection = FVector(-Direction.Y, Direction.X, Direction.Z);

	if (FormationPositioningType == EMMOFormationPositioningType::LeftRight)
	{
		FVector Temp = Direction;
		Direction = -NormalDirection;
		NormalDirection = Temp;
	}

	const TArray<FVector2D>& Tank2Ds = Setup.GetOffsets(EMMOCharacterRole::Tank);
	const TArray<FVector2D>& Melee2Ds = Setup.GetOffsets(EMMOCharacterRole::Melee);
	const TArray<FVector2D>& Ranged2Ds = Setup.GetOffsets(EMMOCharacterRole::Ranged);
	const TArray<FVector2D>& Healer2Ds = Setup.GetOffsets(EMMOCharacterRole::Healer);

	TArray<FVector> TankOffsets;
	for (FVector2D Tank2D : Tank2Ds){ TankOffsets.Add(NormalDirection * Tank2D.X - Direction * (Tank2D.Y)); }

	TArray<FVector> MeleeOffsets;
	for (FVector2D Melee2D : Melee2Ds){ MeleeOffsets.Add(NormalDirection * Melee2D.X - Direction * (Melee2D.Y)); }

	TArray<FVector> RangedOffsets;
	for (FVector2D Ranged2D : Ranged2Ds){ RangedOffsets.Add(NormalDirection * Ranged2D.X - Direction * (Ranged2D.Y)); }
	
	TArray<FVector> HealerOffsets;
	for (FVector2D Healer2D : Healer2Ds){ HealerOffsets.Add(NormalDirection * Healer2D.X - Direction * (Healer2D.Y)); }

	// Each group is disposed like a simple formation. so we have 4 or more simple formation.

	// Generate tank anchor points
	TArray<FMMOFormationPoint> TankFormationPoints;
	for (const FVector& TankOffset : TankOffsets) TankFormationPoints.Add(FMMOFormationPoint(AnchorPoint + TankOffset, LastPoint + TankOffset));

	TArray<FVector> TankPoints = ComputeSimpleFormation_Internal(FormationTuning, Tanks.Num(), TankFormationPoints);

	// Generate melee anchor points
	TArray<FMMOFormationPoint> MeleeFormationPoints;
	for (const FVector& MeleeOffset : MeleeOffsets) MeleeFormationPoints.Add(FMMOFormationPoint(AnchorPoint + MeleeOffset, LastPoint + MeleeOffset));

	TArray<FVector> MeleePoints = ComputeSimpleFormation_Internal(FormationTuning, Melee.Num(), MeleeFormationPoints);
	
	// Generate ranged anchor points
	TArray<FMMOFormationPoint> RangedFormationPoints;
	for (const FVector& RangedOffset : RangedOffsets) RangedFormationPoints.Add(FMMOFormationPoint(AnchorPoint + RangedOffset, LastPoint + RangedOffset));
	
	TArray<FVector> RangedPoints = ComputeSimpleFormation_Internal(FormationTuning, Ranged.Num(), RangedFormationPoints);
	
	// Generate healer anchor points
	TArray<FMMOFormationPoint> HealerFormationPoints;
	for (const FVector& HealerOffset : HealerOffsets) HealerFormationPoints.Add(FMMOFormationPoint(AnchorPoint + HealerOffset, LastPoint + HealerOffset));

	TArray<FVector> HealerPoints = ComputeSimpleFormation_Internal(FormationTuning, Healers.Num(), HealerFormationPoints);

	// Sort points so we dont have problem with path finding. (or just have less problem)
	TankPoints = SortPoints(Tanks, TankPoints, AnchorPoint);
	MeleePoints = SortPoints(Melee, MeleePoints, AnchorPoint);
	RangedPoints = SortPoints(Ranged, RangedPoints, AnchorPoint);
	HealerPoints = SortPoints(Healers, HealerPoints, AnchorPoint);

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

FMMOFormationSetup UMMOFormationManager::HandleFormationBiases(const FMMOFormationSetup& Setup) const
{
	// Offset Y so we dont click and position heroes far away.
	float MinY = 10.f;
	float MinX = 10.f;

	TArray<FVector2D> Tank2Ds = Setup.GetOffsets(EMMOCharacterRole::Tank);
	TArray<FVector2D> Melee2Ds = Setup.GetOffsets(EMMOCharacterRole::Melee);
	TArray<FVector2D> Ranged2Ds = Setup.GetOffsets(EMMOCharacterRole::Ranged);
	TArray<FVector2D> Healer2Ds = Setup.GetOffsets(EMMOCharacterRole::Healer);

	for (FVector2D Tank2D : Tank2Ds) 
	{ 
		MinX = FMath::Min(Tank2D.X, MinX); 
		MinY = FMath::Min(Tank2D.Y, MinY);
	}

	for (FVector2D Melee2D : Melee2Ds)
	{
		MinX = FMath::Min(Melee2D.X, MinX);
		MinY = FMath::Min(Melee2D.Y, MinY);
	}

	for (FVector2D Ranged2D : Ranged2Ds)
	{
		MinX = FMath::Min(Ranged2D.X, MinX);
		MinY = FMath::Min(Ranged2D.Y, MinY);
	}

	for (FVector2D Healer2D : Healer2Ds)
	{
		MinX = FMath::Min(Healer2D.X, MinX);
		MinY = FMath::Min(Healer2D.Y, MinY);
	}

	auto ApplyBias = [](TArray<FVector2D>& ArrayRef, float X, float Y)
	{
		for (FVector2D& Value : ArrayRef)
		{
			Value.X -= X;
			Value.Y -= Y;
		}
	};

	if (FormationPositioningType == EMMOFormationPositioningType::UpDown)
		MinX = 0.f;

	ApplyBias(Tank2Ds, MinX, MinY);
	ApplyBias(Melee2Ds, MinX, MinY);
	ApplyBias(Ranged2Ds, MinX, MinY);
	ApplyBias(Healer2Ds, MinX, MinY);

	FMMOFormationSetup BiasSetup;
	BiasSetup.Add(EMMOCharacterRole::Tank, Tank2Ds);
	BiasSetup.Add(EMMOCharacterRole::Melee, Melee2Ds);
	BiasSetup.Add(EMMOCharacterRole::Ranged, Ranged2Ds);
	BiasSetup.Add(EMMOCharacterRole::Healer, Healer2Ds);

	return BiasSetup;
}

TArray<FTransform> UMMOFormationManager::ComputeFormation(const TArray<AMMOBaseHero*>& Heroes, FVector AnchorPoint, FVector LastPoint, bool bShowPreview)
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
		Points = ComputeSimpleFormation_Internal(SimpleFormationTuning, Heroes.Num(), AnchorPoint, LastPoint);
		Points = SortPoints(Heroes, Points, AnchorPoint);
	}
	else if (FormationType == EMMOFormationType::Standard)
	{
		// Offset X-Y so we dont click and position heroes far away from click position.
		FMMOFormationSetup StandardBiasSetup = HandleFormationBiases(StandardSetup);
		Points = ComputeAdvancedFormation_Internal(AdvancedFormationTuning, StandardBiasSetup, Heroes, AnchorPoint, LastPoint);
	}
	else if (FormationType == EMMOFormationType::Advanced)
	{
		const FMMOFormationSetup& FormationSetup = HandleFormationBiases(HasValidAdvancedFormation() ? AdvancedSetupFormation : StandardSetup);
		Points = ComputeAdvancedFormation_Internal(AdvancedFormationTuning, FormationSetup, Heroes, AnchorPoint, LastPoint);
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

	if (bShowPreview)
	{
		ShowPreview(Points);
	}

	check(Points.Num() == Heroes.Num());

	const FRotator Rotation = (LastPoint - AnchorPoint).GetSafeNormal().ToOrientationRotator();
	TArray<FTransform> Transforms;
	Algo::Transform(Points, Transforms, [&Rotation](const FVector& Point) { return FTransform(Rotation, Point, FVector::OneVector); });

	return Transforms;
}

TArray<FVector> UMMOFormationManager::SortPoints(const TArray<AMMOBaseHero*>& Heroes, const TArray<FVector>& Points, const FVector& AnchorPoint) const
{
	if (FormationSortType == EMMOFormationSortType::Nearest)
	{
		return SortPoints_Nearest(Heroes, Points, AnchorPoint);
	}
	else if (FormationSortType == EMMOFormationSortType::Similar)
	{
		return SortPoints_Similiar(Heroes, Points, AnchorPoint);
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

TArray<FVector2D> FMMOFormationSetup::GetOffsets(EMMOCharacterRole CharacterRole) const
{
	if (GroupsPositionOffset.Contains(CharacterRole))
	{
		TArray<FVector2D> Offsets = GroupsPositionOffset[CharacterRole].Array;
		
		for (FVector2D& Offset : Offsets)
		{
			Offset.X = FMath::Clamp(Offset.X, -1.f, 1.f);
			Offset.Y = FMath::Clamp(Offset.Y, -1.f, 1.f);
		}

		return Offsets;
	}

	check(0);
	return {};
}

int32 FMMOFormationTuning::GetMaxHeroesPerRow(float Distance) const
{
	if (Distance < MapDistance.X)
	{
		return MaxHeroesPerRow.X;
	}

	if (Distance > MapDistance.Y)
	{
		return MaxHeroesPerRow.Y;
	}

	const float Alpha = FMath::GetRangePct(MapDistance, Distance);

	return FMath::Lerp(MaxHeroesPerRow.X, MaxHeroesPerRow.Y, Alpha);
}

float FMMOFormationTuning::GetHorizontalMargin(float Distance) const
{
	return FMath::GetMappedRangeValueClamped(MapDistance, HorizontalMargin, Distance);
}

float FMMOFormationTuning::GetVerticalMargin(float Distance) const
{
	return FMath::GetMappedRangeValueClamped(MapDistance, VerticalMargin, Distance);
}
