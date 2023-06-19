// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/MMOBaseCharacter.h"
#include "MMOBaseHero.generated.h"

class UDecalComponent;

UENUM(BlueprintType)
enum class EMMOGuildRole : uint8
{
	GuildMaster,
	Officier,
	Raider,
	New
};

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

	UFUNCTION(BlueprintCallable, Category = Preview)
	void ShowSkillRange(const int32 Index);

	UFUNCTION(BlueprintCallable, Category = Preview)
	void HideSkillRange();

	UPROPERTY(EditAnywhere, Category = Role)
	EMMOGuildRole GuildRole = EMMOGuildRole::New;
private:
	/** Selection Decal */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UDecalComponent* SelectionDecal;

	/** Skill Preview Decal */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UDecalComponent* SkillPreviewDecal;

	bool CanShowSkillPreview() const;

	int32 CurrentSkillPreviewIdx = -1;
};
