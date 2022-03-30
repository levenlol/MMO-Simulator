// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MMOFloatingTextActor.h"
#include "Components/WidgetComponent.h"

// Sets default values
AMMOFloatingTextActor::AMMOFloatingTextActor()
	: Super()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	WidgetComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	WidgetComponent->SetCollisionProfileName(FName("NoCollision"));
}

// Called when the game starts or when spawned
void AMMOFloatingTextActor::BeginPlay()
{
	Super::BeginPlay();
	
	AnchorLocation = GetActorLocation();
}

// Called every frame
void AMMOFloatingTextActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

