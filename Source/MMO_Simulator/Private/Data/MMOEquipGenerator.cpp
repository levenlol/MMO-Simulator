// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/MMOEquipGenerator.h"
#include "MMOGameInstance.h"

UMMOEquipGenerator* UMMOEquipGenerator::Instance = nullptr;

void UMMOEquipGenerator::Startup(UObject* Outer)
{
	check(!Instance);
	Instance = NewObject<UMMOEquipGenerator>(Outer);
	Instance->AddToRoot();
	Instance->Init();
}

void UMMOEquipGenerator::Shutdown()
{
	check(Instance);
	Instance->Uninit();
	Instance->RemoveFromRoot();
	Instance = nullptr;
}

void UMMOEquipGenerator::Init()
{

}

void UMMOEquipGenerator::Uninit()
{
}

