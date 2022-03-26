// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/MMOBaseEquip.h" 
#include "Core/MMOCommon.h"
#include "MMOBaseWeapon.generated.h"

class UAnimSequenceBase;

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

	// Custom animation. might be used for bosses or legendary weapons.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	UAnimSequenceBase* CustomWeaponAnimation = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
