// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/MMOBaseCharacter.h"
#include "MMOBaseHero.generated.h"

class UDecalComponent;


UCLASS()
class MMO_SIMULATOR_API AMMOBaseHero : public AMMOBaseCharacter
{
	GENERATED_BODY()
public:
	
	AMMOBaseHero();


	UFUNCTION(BlueprintNativeEvent, Category = Selection)
	void OnSelected();

	UFUNCTION(BlueprintNativeEvent, Category = Selection)
	void OnDeselected();

	UFUNCTION(BlueprintPure, Category = Decal)
	FORCEINLINE UDecalComponent* GetDecalComponent() const { return SelectionDecal; }

private:
	/** A decal to follow  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UDecalComponent* SelectionDecal;
};
