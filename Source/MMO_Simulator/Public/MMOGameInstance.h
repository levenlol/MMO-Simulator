// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Core/MMOCommon.h"
#include "Engine/DataTable.h"
#include "MMOGameInstance.generated.h"

class UDataTable;
class UAnimSequenceBase;
class UMMOGuildsManager;

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMOAnimationDataTable : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	FMMOWeaponTypeCouple WeaponsType;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimSequenceBase* WeaponAnimation;
};

UCLASS()
class MMO_SIMULATOR_API UMMOGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	virtual void Init() override;
	virtual void Shutdown() override;

	UFUNCTION(BlueprintPure, meta = (WorldContext = WorldContextObject))
	static UMMOGameInstance* GetMMOGameInstance(const UObject* WorldContextObject);

	UDataTable* RetrieveDataTable(FName Key) const;

	UPROPERTY(EditDefaultsOnly, Category = Data)
	TMap<FName, UDataTable*> DataTableMaps;
	
	UPROPERTY(EditDefaultsOnly, Category = Guild)
	TSubclassOf<UMMOGuildsManager> GuildsManagerClass;

	UMMOGuildsManager* GetGuildsManager() const { return GuildsManager; }
private:
	UPROPERTY(VisibleAnywhere, Category = Guild)
	UMMOGuildsManager* GuildsManager;

};
