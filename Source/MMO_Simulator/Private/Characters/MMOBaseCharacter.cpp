// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/MMOBaseCharacter.h"

// Sets default values
AMMOBaseCharacter::AMMOBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMMOBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(RecuperateTimerHandle, this, &AMMOBaseCharacter::OnRecuperate, static_cast<float>(Stats.RecuperateEverySeconds), true, -1.f);
}

void AMMOBaseCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(RecuperateTimerHandle);
}

void AMMOBaseCharacter::OnRecuperate()
{
	Stats.Recuperate(Stats.RecuperateEverySeconds);
}

void AMMOBaseCharacter::DamageTake_Implementation(FMMODamage InDamage)
{

}

// Called every frame
void AMMOBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
