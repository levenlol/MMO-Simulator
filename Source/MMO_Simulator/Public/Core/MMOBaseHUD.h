// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Core/MMOPlayerController.h"
#include "MMOBaseHUD.generated.h"

/**
 * 
 */
UCLASS()
class MMO_SIMULATOR_API AMMOBaseHUD : public AHUD
{
	GENERATED_BODY()
public:
	AMMOBaseHUD();

	virtual void DrawHUD() override;


	UPROPERTY(EditDefaultsOnly, Category = Selection)
	UTexture* SelectionTexture = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Selection)
	float TextureAlpha = 0.5f;

private:
	FORCEINLINE AMMOPlayerController* GetOwningPlayerController() const { return Cast<AMMOPlayerController>(PlayerOwner.Get()); };
};
