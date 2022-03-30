// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MMOFloatingTextActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMMOOnAnimationFinished, AMMOFloatingTextActor*, Sender);

class UWidgetComponent;

UCLASS(ABSTRACT)
class MMO_SIMULATOR_API AMMOFloatingTextActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMMOFloatingTextActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UI)
	UWidgetComponent* WidgetComponent;

	UFUNCTION(blueprintImplementableEvent, Category = "FloatingTextActor")
	void Initialize(const FText& text);

	FORCEINLINE const FVector& GetAnchorLocation() const { return AnchorLocation; }
	FORCEINLINE void SetAnchorLocation(const FVector& InLocation) { AnchorLocation = InLocation; }

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = Anim)
	FMMOOnAnimationFinished OnAnimationFinished;

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
