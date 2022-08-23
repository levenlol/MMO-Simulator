// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/MMOCommon.h"
#include "MMOFormationManager.generated.h"

class AMMOBaseHero;

UENUM(BlueprintType)
enum class EMMOFormationSortType : uint8
{
	None,
	Nearest,
	Similar
};

UENUM(BlueprintType)
enum class EMMOFormationType : uint8
{
	Simple,
	Standard,
	Advanced
};

UENUM(BlueprintType)
enum class EMMOFormationPositioningType : uint8
{
	UpDown,
	LeftRight
};

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMOVector2Container
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	TArray<FVector2D> Array;
};

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMOFormationSetup
{
	GENERATED_BODY()
public:
	FMMOFormationSetup() {}
	void Add(EMMOCharacterRole Role, const TArray<FVector2D>& InOffsets)
	{
		GroupsPositionOffset.FindOrAdd(Role).Array = InOffsets;
	}

	UPROPERTY(BlueprintReadWrite, Category = Formation)
	TMap<EMMOCharacterRole, FMMOVector2Container> GroupsPositionOffset;

	TArray<FVector2D> GetOffsets(EMMOCharacterRole CharacterRole) const;
};

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMOFormationPoint
{
	GENERATED_BODY()
public:
	FMMOFormationPoint() {}
	FMMOFormationPoint(const FVector& InAnchorPoint, const FVector& InLastPoint)
		: AnchorPoint(InAnchorPoint)
		, LastPoint(InLastPoint)
	{}

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Point)
	FVector AnchorPoint = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Point)
	FVector LastPoint = FVector::OneVector;
};

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMOFormationTuning
{
	GENERATED_BODY()
public:
	int32 GetMaxHeroesPerRow(float Distance) const;
	float GetHorizontalMargin(float Distance) const;
	float GetVerticalMargin(float Distance) const;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup, meta = (ClampMin = "1", UIMin = "1"))
	FIntPoint MaxHeroesPerRow = FIntPoint(2, 5);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
	FVector2D HorizontalMargin = FVector2D(200.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
	FVector2D VerticalMargin = FVector2D(150.f);

	// Distance to lerp between min max. If value < min X is taken, if value > max Y is taken. lerp otherwise
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
	FVector2D MapDistance = FVector2D(100.f, 500.f);
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MMO_SIMULATOR_API UMMOFormationManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMMOFormationManager();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Number of pre-spawned actors.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = UI)
	int32 InitialPreviewsCacheSize = 25;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
	FMMOFormationTuning SimpleFormationTuning;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
	FMMOFormationTuning AdvancedFormationTuning;

	// Max distance from roles
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup, meta = (ClampMin = "100", UIMin = "100"))
	float AdvancedFormationLength = 1500.f; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup, meta = (ClampMin = "50", UIMin = "50"))
	float FormationThreshold = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setup)
	EMMOFormationSortType FormationSortType = EMMOFormationSortType::Similar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setup)
	EMMOFormationType FormationType = EMMOFormationType::Simple;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setup)
	EMMOFormationPositioningType FormationPositioningType = EMMOFormationPositioningType::UpDown;

	// return an array of points, each for every actors passed as input
	UFUNCTION(BlueprintPure, Category = Formation)
	TArray<FVector> ComputeSimpleFormation(const int32 HeroesNum, const FVector& AnchorPoint, FVector LastPoint, bool bShowPreview = true);

	UFUNCTION(BlueprintPure, Category = Formation)
	TArray<FTransform> ComputeFormation(const TArray<AMMOBaseHero*>& Heroes, FVector AnchorPoint, FVector LastPoint, bool bShowPreview = true);

	UFUNCTION(BlueprintPure, Category = Formation)
	TArray<FVector> SortPoints(const TArray<AMMOBaseHero*>& Heroes, const TArray<FVector>& Points, const FVector& AnchorPoint) const;

	UFUNCTION(BlueprintPure, Category = Formation)
	bool HasValidAdvancedFormation() const;

	UFUNCTION(BlueprintCallable, Category = Formation)
	void SetAdvancedFormation(const FMMOFormationSetup& InSetup);

	UFUNCTION(BlueprintCallable, Category = UI)
	void ShowPreview(const TArray<FVector>& Points);

	void ShowPreview(FVector Point);

	UFUNCTION(BlueprintCallable, Category = UI)
	void HideAllPreviews();

protected:
	// Formation Preview Actor class.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UI)
	TSubclassOf<AActor> UIFormationPreviewClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UI)
	FMMOFormationSetup AdvancedSetupFormation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UI)
	FMMOFormationSetup StandardSetup;

	// FX preview for current formation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = UI)
	TArray<AActor*> UIFormationPreviews;

#if WITH_EDITORONLY_DATA 
	UPROPERTY(EditAnywhere, Category = Debug)
	bool bDebug = false;
#endif
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	int32 CurrentPreviewIndex = -1;

	void AddPrecachePreviewActors(const int32 PreviewNumbersToAdd);
	FVector ProjectPointToNavMesh(const FVector& InLocation) const;

	// greedy find the nearest point to each actor (cannot select the same point)
	TArray<FVector> SortPoints_Nearest(TArray<AMMOBaseHero*> Heroes, TArray<FVector> Points,const FVector& AnchorPoint) const;

	// based on similiarty. 1 - abs(x1 - x2) / (x1 + x2)
	TArray<FVector> SortPoints_Similiar(TArray<AMMOBaseHero*> Heroes, TArray<FVector> Points, const FVector& AnchorPoint) const;

	// Generate a Formation in a rectangular shape
	TArray<FVector> ComputeSimpleFormation_Internal(const FMMOFormationTuning& FormationTuning, const int32 CharactersNum, const FVector& AnchorPoint, const FVector& LastPoint) const;
	TArray<FVector> ComputeSimpleFormation_Internal(const FMMOFormationTuning& FormationTuning, const int32 CharactersNum, const TArray<FMMOFormationPoint>& AnchorPoints) const;

	// Generate SimpleFormation at given offset
	TArray<FVector> ComputeAdvancedFormation_Internal(const FMMOFormationTuning& FormationTuning, FMMOFormationSetup Setup, const TArray<AMMOBaseHero*>& Heroes, const FVector& AnchorPoint, const FVector& LastPoint);

	FMMOFormationSetup HandleFormationBiases(const FMMOFormationSetup& Setup, bool bConsiderTank, bool bConsiderMelee, bool bConsiderRanged, bool bConsiderHealer) const;
};
