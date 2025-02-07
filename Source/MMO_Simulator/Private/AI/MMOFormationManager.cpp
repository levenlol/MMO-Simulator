// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MMOFormationManager.h"
#include "Characters/MMOBaseHero.h"
#include "NavigationSystem.h"

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

	ANavigationData* NavData = NavigationSystem->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);
	if (!NavData)
	{
		UE_LOG(LogTemp, Error, TEXT("FormationManager: cannot find ANavigationData"));
		return InLocation;
	}

	FNavLocation EndLocation;
	if (NavigationSystem->ProjectPointToNavigation(InLocation, EndLocation, FVector(500.f)))
	{
		return EndLocation.Location;
	}

	return InLocation;
}

void UMMOFormationManager::ShowPreview(const TArray<FVector>& Points)
{
	for (const FVector& Point : Points)
	{
		ShowPreview(Point);
	}
}

void UMMOFormationManager::ShowPreview(const FVector& Point)
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

TArray<FVector> UMMOFormationManager::ComputeFormation(const int32 CharactersNum, const FVector& AnchorPoint, const FVector& LastPoint, bool bShowPreview /* = true */)
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

