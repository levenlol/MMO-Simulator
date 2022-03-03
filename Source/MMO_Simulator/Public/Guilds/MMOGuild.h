// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Core/MMOCommon.h"
#include "MMOGuild.generated.h"

class UTexture2D;

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMOGuildMember
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	FName MemberName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	TSoftObjectPtr<UTexture2D> MemberImage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	FMMOCharacter CharacterInfo;
};

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMOGuild
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	FName GuildName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	TSoftObjectPtr<UTexture2D> GuildIcon;
	
};
