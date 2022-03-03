// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MMOBaseBuilding.generated.h"

class UStaticMeshComponent;

UCLASS()
class MMO_SIMULATOR_API AMMOBaseBuilding : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMMOBaseBuilding();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Default)
	UStaticMeshComponent* Mesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
