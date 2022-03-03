// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Buildings/MMOBaseBuilding.h"
#include "MMOPreviewBuilding.generated.h"

class UStaticMesh;
class UMaterialInterface;

UCLASS()
class MMO_SIMULATOR_API AMMOPreviewBuilding : public AMMOBaseBuilding
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = Building)
	void SetPreviewMesh(UStaticMesh* InMesh);

	UFUNCTION(BlueprintPure, Category = Building)
	bool IsColliding() const;

	UPROPERTY(EditAnywhere, Category = Building)
	UMaterialInterface* PreviewMaterial;
private:
	void ChangeMeshColor();
};
