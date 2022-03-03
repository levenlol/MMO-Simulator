// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MMOGuild.h"

UMMOGuild* UMMOGuild::Instance = nullptr;

void UMMOGuild::Startup(UObject* Outer)
{
	check(!Instance);
	Instance = NewObject<UMMOGuild>(Outer);
	Instance->AddToRoot();
	Instance->Init();
}

void UMMOGuild::Shutdown()
{
	check(Instance);
	Instance->Uninit();
	Instance->RemoveFromRoot();
	Instance = nullptr;
}

void UMMOGuild::Init()
{
}

void UMMOGuild::Uninit()
{
}
