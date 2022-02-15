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
	
}

// Called every frame
void AMMOBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
