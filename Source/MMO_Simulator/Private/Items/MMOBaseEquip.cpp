// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/MMOBaseEquip.h"
#include "Components/StaticMeshComponent.h"

AMMOBaseEquip::AMMOBaseEquip()
	: Super()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);
	WeaponMesh->SetGenerateOverlapEvents(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	WeaponMesh->SetCollisionProfileName(FName("NoCollision"));
}
