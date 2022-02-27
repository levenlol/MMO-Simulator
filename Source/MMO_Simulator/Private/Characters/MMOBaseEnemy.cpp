// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/MMOBaseEnemy.h"
#include "AI/MMOAggroManagerComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"

AMMOBaseEnemy::AMMOBaseEnemy()
	: Super()
{
	GetCapsuleComponent()->SetCollisionProfileName(FName("EnemyPawn"));

	AggroManager = CreateDefaultSubobject<UMMOAggroManagerComponent>(TEXT("AggroManager"));
	
	AggroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AggroSphere"));
	AggroSphere->SetupAttachment(RootComponent);
}

void AMMOBaseEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AMMOBaseEnemy::BeginPlay()
{
	Super::BeginPlay();
}
