// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MMOFloatingCombatTextComponent.generated.h"

class AMMOFloatingTextActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MMO_SIMULATOR_API UMMOFloatingCombatTextComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMMOFloatingCombatTextComponent();

private:
	// the floating combat text actor to use to display the actor
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FloatingCombatTextComponent", Meta = (AllowPrivateAccess = "True"))
	TSubclassOf<AMMOFloatingTextActor> FloatingTextActorClass = nullptr;

	// the amount of vertical space between each damage number
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FloatingCombatTextComponent", Meta = (AllowPrivateAccess = "True"))
	float TextVerticalSpacing = 40.0f;

	// The amount of vertical offset to apply when spawning a new text actor
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FloatingCombatTextComponent", Meta = (AllowPrivateAccess = "True"))
	float TextVerticalOffset = 100.0f;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// adds the specified text at the specified location in world space
	UFUNCTION(BlueprintCallable, Category = "FloatingCombatTextComponent")
	void AddFloatingText(const FText& text, FVector world_Location);

private:
	// fallback for when a floating text actor is destroyed
	UFUNCTION()
	void OnTextDestroyed(AActor* destroyed_actor);

	// An array of all active floating text actors
	UPROPERTY()
	TArray<AMMOFloatingTextActor*> ActiveTextActor;
};
