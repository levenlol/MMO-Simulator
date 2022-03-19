// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Core/MMOCommon.h"
#include "MMOGameInstance.generated.h"

class UDataTable;
class UMMOGuildsManager;


UCLASS()
class MMO_SIMULATOR_API UMMOGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	virtual void Init() override;
	virtual void Shutdown() override;

	UFUNCTION(BlueprintPure, meta = (WorldContext = WorldContextObject))
	static UMMOGameInstance* GetMMOGameInstance(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = Dungeon)
	void AddCharacterForRaid(const FMMOCharacter& InCharacter);

	UFUNCTION(BlueprintPure, Category = Dungeon)
	const TArray<FMMOCharacter>& GetRaidCharacters() const { return RaidCharacters; }

	UDataTable* RetrieveDataTable(FName Key) const;

	UPROPERTY(EditDefaultsOnly, Category = Data)
	TMap<FName, UDataTable*> DataTableMaps;
	
	UPROPERTY(EditDefaultsOnly, Category = Guild)
	TSubclassOf<UMMOGuildsManager> GuildsManagerClass;

	UMMOGuildsManager* GetGuildsManager() const { return GuildsManager; }
private:
	UPROPERTY(VisibleAnywhere, Category = Guild)
	UMMOGuildsManager* GuildsManager;

	UPROPERTY(VisibleAnywhere, Category = Dungeon)
	TArray<FMMOCharacter> RaidCharacters;
};
