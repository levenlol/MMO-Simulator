// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MMOFunctionLibrary.generated.h"

struct MMO_SIMULATOR_API FMMOPropertyBlob
{
public:
	void* Leaf = nullptr;
	int32 Index = 0;

	FProperty* Property = nullptr;
	FString Path;

	double GetValue() const;

private:
	template<typename ValueType>
	ValueType* GetValue() const;
};

template<typename ValueType>
ValueType* FMMOPropertyBlob::GetValue() const
{
	if (Property)
	{
		ValueType* SourceAddr = Property->ContainerPtrToValuePtr<ValueType>(Leaf, Index);
		return SourceAddr;
	}

	return nullptr;
}

UCLASS()
class MMO_SIMULATOR_API UMMOFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Inspired by MovieSceneCommonHelper::FindPropertyAndArrayIndex
	static FProperty* FindProperty(const UObject* InObject, const FString& PropertyPath, void*& OutLeafObject, int32& OutIndex);
	static FMMOPropertyBlob FindProperty(const UObject* InObject, const FString& PropertyPath);

	static void SetPropertyValue(const UObject* InObject, const FString& PropertyPath, float Value);
	
	template<typename ValueType>
	static ValueType* GetPropertyValue(const UObject* InObject, const FString& PropertyPath);

	static TArray<FMMOPropertyBlob> ListProperties(const UObject* InObject, const TArray<FName>& ClassFilters = {});
	static TArray<FMMOPropertyBlob> ListProperties(UStruct* InClass, const TArray<FName>& ClassFilters = {});

	static FMMOPropertyBlob FindPropertyOfType(UStruct* InClass, const FString& PropertyType);
	static TArray<FMMOPropertyBlob> FindPropertiesOfType(UStruct* InClass, const FString& PropertyType);

private:
	static void ListProperties_Recursive(UStruct* Struct, FString Path, TArray<FMMOPropertyBlob>& OutProperties, const TArray<FName>& ClassFilters);

	static void ParsePropertyForIndex(const FString& InProperty, FString& OutName, int32& OutIndex);
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
