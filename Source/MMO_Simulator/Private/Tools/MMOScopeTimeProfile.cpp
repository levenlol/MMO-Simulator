// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/MMOScopeTimeProfile.h"

FMMOScopeTimeProfile::FMMOScopeTimeProfile(FString InLabel)
	: Label(InLabel)
{
	begin = std::chrono::steady_clock::now();
}

FMMOScopeTimeProfile::~FMMOScopeTimeProfile()
{
	end = std::chrono::steady_clock::now();

	auto Elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
	float micro = static_cast<float>(Elapsed / 1000.f);
	UE_LOG(LogTemp, Warning, TEXT("%s: ElapedTime: %f us"), *Label, micro);
}