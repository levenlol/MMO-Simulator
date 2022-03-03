// Fill out your copyright notice in the Description page of Project Settings.


#include "Buildings/MMOBaseBuilding.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AMMOBaseBuilding::AMMOBaseBuilding()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
}

// Called when the game starts or when spawned
void AMMOBaseBuilding::BeginPlay()
{
	Super::BeginPlay();
	
}
