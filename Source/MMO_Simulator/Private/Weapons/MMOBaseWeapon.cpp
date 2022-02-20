// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/MMOBaseWeapon.h"

AMMOBaseWeapon::AMMOBaseWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);
	WeaponMesh->SetGenerateOverlapEvents(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	WeaponMesh->SetCollisionProfileName(FName("NoCollision"));
}

void AMMOBaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}
