// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/MMOBaseEquip.h" 
#include "Core/MMOCommon.h"
#include "MMOBaseWeapon.generated.h"


UCLASS()
class MMO_SIMULATOR_API AMMOBaseWeapon : public AMMOBaseEquip
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMMOBaseWeapon();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	EMMOWeaponType WeaponType;

	// Stats given by the weapon
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	FMMOWeaponStats Stats;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
