// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/MMOBaseItem.h"
#include "Core/MMOCommon.h"
#include "MMOBaseEquip.generated.h"

class UStaticMeshComponent;

UCLASS()
class MMO_SIMULATOR_API AMMOBaseEquip : public AMMOBaseItem
{
	GENERATED_BODY()
public:

	AMMOBaseEquip();

	UPROPERTY(VisibleAnywhere, Category = Mesh)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats)
	FMMOItemStats EquipStats;
};
