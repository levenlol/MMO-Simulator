// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MMOFormationManager.generated.h"

class AMMOBaseHero;

UENUM(BlueprintType)
enum class EMMOFormationSortType : uint8
{
	None,
	Nearest,
	Similar
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

	// return an array of points, each for every actors passed as input
	UFUNCTION(BlueprintPure, Category = Formation)
	TArray<FVector> ComputeSimpleFormation(const int32 CharactersNum, const FVector& AnchorPoint, const FVector& LastPoint, bool bShowPreview = true);

	UFUNCTION(BlueprintPure, Category = Formation)
	TArray<FVector> ComputeFormation(const TArray<AMMOBaseHero*>& Heroes, const FVector& AnchorPoint, FVector LastPoint, bool bShowPreview = true);

	UFUNCTION(BlueprintCallable, Category = UI)
	void ShowPreview(const TArray<FVector>& Points);

	void ShowPreview(FVector Point);

	UFUNCTION(BlueprintCallable, Category = UI)
	void HideAllPreviews();

protected:
	// Formation Preview Actor class.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UI)
	TSubclassOf<AActor> UIFormationPreviewClass;

	// FX preview for current formation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = UI)
	TArray<AActor*> UIFormationPreviews;

	// Number of pre-spawned actors.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = UI)
	int32 InitialPreviewsCacheSize = 25;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup, meta = (ClampMin = "1", UIMin = "1"))
	int32 MaxHeroesPerRow = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
	float HorizontalMargin = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
	float VerticalMargin = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
	EMMOFormationSortType FormationSortType = EMMOFormationSortType::Similar;

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
	TArray<FVector> SortPoints_Nearest(TArray<AMMOBaseHero*> Heroes, TArray<FVector> Points,const FVector& AnchorPoint, const FVector& LastPoint);

	// based on similiarty. 1 - abs(x1 - x2) / (x1 + x2)
	TArray<FVector> SortPoints_Similiar(TArray<AMMOBaseHero*> Heroes, TArray<FVector> Points, const FVector& AnchorPoint, const FVector& LastPoint);
};
