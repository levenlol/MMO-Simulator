// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MMOBuildingManager.generated.h"

class AMMOBaseBuilding;
class AMMOPreviewBuilding;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MMO_SIMULATOR_API UMMOBuildingManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMMOBuildingManager();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, Category = Building)
	TMap<FName, TSubclassOf<AMMOBaseBuilding>> BuildingsList;

	UPROPERTY(EditAnywhere, Category = Building)
	TSubclassOf<AMMOPreviewBuilding> PreviewBuilding;
protected:
	virtual void BeginPlay() override;
};
