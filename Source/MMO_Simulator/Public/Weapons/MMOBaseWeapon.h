// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/MMOCommon.h"
#include "MMOBaseWeapon.generated.h"


UCLASS()
class MMO_SIMULATOR_API AMMOBaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMMOBaseWeapon();

	UPROPERTY(VisibleAnywhere, Category = Mesh)
	UStaticMeshComponent* WeaponMesh;

	// Weapon Type
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	EMMOWeaponType WeaponType;

	// Stats given by the weapon
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	FMMOWeaponStats Stats;

	// 0 - 100
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon, meta = (ClampMin = "0", ClampMax = "100"))
	int32 CritChance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
