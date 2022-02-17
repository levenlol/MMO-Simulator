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
	TArray<FVector> ComputeFormation(const TArray<AMMOBaseHero*>& InHeroes, const FVector& AnchorPoint, const FVector& LastPoint);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup, meta = (ClampMin = "1", UIMin = "1"))
	int32 MaxHeroesPerRow = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
	float HorizontalMargin = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
	float VerticalMargin = 150.f;

	// Called when the game starts
	virtual void BeginPlay() override;
		
};
