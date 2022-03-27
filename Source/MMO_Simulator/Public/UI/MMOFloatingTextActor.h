// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MMOFloatingTextActor.generated.h"

UCLASS(ABSTRACT)
class MMO_SIMULATOR_API AMMOFloatingTextActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMMOFloatingTextActor();

	UFUNCTION(blueprintImplementableEvent, Category = "FloatingTextActor")
	void Initialize(const FText& text);

	inline const FVector& GetAnchorLocation() const { return AnchorLocation; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:
	//The lcoation to which the text is anchored
	FVector AnchorLocation;

};
