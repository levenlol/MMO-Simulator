// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MMOGroupsManagerComponent.generated.h"

class AMMOBaseHero;

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMOGroup
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TArray<AMMOBaseHero*> Heroes;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MMO_SIMULATOR_API UMMOGroupsManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMMOGroupsManagerComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = Group)
	void AddGroup(FName InName, FMMOGroup Group);

	UFUNCTION(BlueprintPure, Category = Group)
	FORCEINLINE bool HasGroup(FName Name) const { return Groups.Contains(Name); }

	UFUNCTION(BlueprintPure, Category = Group)
	const FMMOGroup& GetGroup(FName Name) const;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TMap<FName, FMMOGroup> Groups;
};
