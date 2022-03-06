// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <chrono>


struct MMO_SIMULATOR_API FMMOScopeTimeProfile 
{
public:
	FMMOScopeTimeProfile(FString InLabel);
	~FMMOScopeTimeProfile();
private:
	FString Label;

	std::chrono::steady_clock::time_point begin;
	std::chrono::steady_clock::time_point end;
};
