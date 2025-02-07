// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MMOFormationManager.generated.h"

class AMMOBaseHero;

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
	UFUNCTION(BlueprintCallable, Category = Formation)
	TArray<FVector> ComputeFormation(const int32 CharactersNum, const FVector& AnchorPoint, const FVector& LastPoint, bool bShowPreview = true);

	UFUNCTION(BlueprintCallable, Category = UI)
	void ShowPreview(const TArray<FVector>& Points);

	void ShowPreview(const FVector& Point);

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

	// Called when the game starts
	virtual void BeginPlay() override;

private:
	int32 CurrentPreviewIndex = -1;

	void AddPrecachePreviewActors(const int32 PreviewNumbersToAdd);
	FVector ProjectPointToNavMesh(const FVector& InLocation) const;
};
