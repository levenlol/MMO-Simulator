// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MMOFunctionLibrary.generated.h"

UCLASS()
class MMO_SIMULATOR_API UMMOFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Inspired by MovieSceneCommonHelper::FindPropertyAndArrayIndex
	static FProperty* FindProperty(const UObject* InObject, const FString& PropertyPath, void*& OutLeafObject, int32& OutIndex);

	static void SetPropertyValue(const UObject* InObject, const FString& PropertyPath, float Value);
	
	template<typename ValueType>
	static ValueType* GetPropertyValue(const UObject* InObject, const FString& PropertyPath);

};

template<typename ValueType>
ValueType* UMMOFunctionLibrary::GetPropertyValue(const UObject* InObject, const FString& PropertyPath)
{
	void* Leaf = nullptr;
	int32 Index = 0;
	FProperty* Property = FindProperty(InObject, PropertyPath, Leaf, Index);

	if (!Property)
		return nullptr;

	ValueType* SourceAddr = Property->ContainerPtrToValuePtr<ValueType>(Leaf, Index);
	return SourceAddr;
}
