// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/MMOBaseEquip.h"
#include "Components/StaticMeshComponent.h"

AMMOBaseEquip::AMMOBaseEquip()
	: Super()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetGenerateOverlapEvents(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	Mesh->SetCollisionProfileName(FName("NoCollision"));
}
