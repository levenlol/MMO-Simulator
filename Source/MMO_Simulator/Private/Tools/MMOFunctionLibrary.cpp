// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/MMOFunctionLibrary.h"

FProperty* UMMOFunctionLibrary::FindProperty(const UObject* InObject, const FString& PropertyPath, void*& OutLeafObject, int32& OutIndex)
{
	if (!InObject)
		return nullptr;

	OutIndex = 0;

	TArray<FString> Properties;
	PropertyPath.ParseIntoArray(Properties, TEXT("."));

	UStruct* Struct = InObject->GetClass();

	OutLeafObject = (void*)InObject;

	// used to handle array path. such as Member1.Member2[3]...
	FString PropertyName;
	int32 PropertyArrayIndex = 0;
	auto ParsePropertyForIndex = [&PropertyName, &PropertyArrayIndex](const FString& InProperty)
	{
		if (InProperty.Len() > 0 && InProperty.GetCharArray()[InProperty.Len() - 1] == ']')
		{
			int32 OpenIndex = 0;
			if (InProperty.FindLastChar('[', OpenIndex))
			{
				FString TruncatedPropertyName(OpenIndex, *InProperty);
				PropertyName = TruncatedPropertyName;

				const int32 NumberLength = InProperty.Len() - OpenIndex - 2;
				if (NumberLength > 0 && NumberLength <= 10)
				{
					TCHAR NumberBuffer[11];
					FMemory::Memcpy(NumberBuffer, &InProperty[OpenIndex + 1], sizeof(TCHAR) * NumberLength);
					LexFromString(PropertyArrayIndex, NumberBuffer);
				}

				return;
			}
		}

		PropertyName = InProperty;
		PropertyArrayIndex = 0;
	};

	auto HandleArrayProp = [&Struct, &OutLeafObject](FArrayProperty* ArrayProp, const int32 ArrayIndex)
	{
		// either one of this two is != nullptr
		FStructProperty* InnerStructProp = CastField<FStructProperty>(ArrayProp->Inner);
		FObjectProperty* InnerObjectProp = CastField<FObjectProperty>(ArrayProp->Inner);

		FScriptArrayHelper InnerHelper{ ArrayProp, ArrayProp->ContainerPtrToValuePtr<void>(OutLeafObject) };
		if (InnerHelper.IsValidIndex(ArrayIndex))
		{
			Struct = InnerStructProp ? (UStruct*)InnerStructProp->Struct : (UStruct*)InnerObjectProp->PropertyClass;
			OutLeafObject = InnerHelper.GetRawPtr(ArrayIndex);
		}
		else
		{
			Struct = nullptr;
			OutLeafObject = nullptr;
		}
	};

	// Walk throught property until the last one
	for (int32 i = 0; i < Properties.Num() - 1; i++)
	{
		if (OutLeafObject == nullptr || Struct == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Find a NULL property in %s."), *PropertyPath)
				return nullptr;
		}

		// Update current variables.
		ParsePropertyForIndex(Properties[i]);

		FProperty* Prop = Struct->FindPropertyByName(*PropertyName);

		// 2 case scenario: array of struct/obj or just a struct/obj
		FArrayProperty* ArrayProp = CastField<FArrayProperty>(Prop);
		if (ArrayProp)
		{
			HandleArrayProp(ArrayProp, PropertyArrayIndex);
		}
		else
		{
			check(PropertyArrayIndex == 0);

			// if not an array, is either a struct or objectptr
			FStructProperty* StructProp = CastField<FStructProperty>(Prop);
			if (StructProp)
			{

				Struct = StructProp->Struct;
				OutLeafObject = StructProp->ContainerPtrToValuePtr<void>(OutLeafObject, PropertyArrayIndex);
			}

			FObjectProperty* ObjProperty = CastField<FObjectProperty>(Prop);
			if (ObjProperty)
			{
				Struct = ObjProperty->PropertyClass;

				void* ObjectContainer = ObjProperty->ContainerPtrToValuePtr<void>(OutLeafObject);
				OutLeafObject = ObjProperty->GetObjectPropertyValue(ObjectContainer);
			}
		}
	}

	// Handle Leaf property.
	ParsePropertyForIndex(Properties.Last());
	OutIndex = PropertyArrayIndex;

	return FindFProperty<FProperty>(Struct, *Properties.Last());
}

void UMMOFunctionLibrary::SetPropertyValue(const UObject* InObject, const FString& PropertyPath, float Value)
{
	if (!InObject)
		return;

	void* Leaf = nullptr;
	int32 Index = 0;
	FProperty* Property = UMMOFunctionLibrary::FindProperty(InObject, PropertyPath, Leaf, Index);

	if (FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property))
	{
		BoolProperty->SetPropertyValue_InContainer(Leaf, Value != 0.f, Index);
	}
	else if (FFloatProperty* FloatProp = CastField<FFloatProperty>(Property))
	{
		FloatProp->SetPropertyValue_InContainer(Leaf, Value, Index);
	}
	else if (FIntProperty* IntProp = CastField<FIntProperty>(Property))
	{
		IntProp->SetPropertyValue_InContainer(Leaf, static_cast<int32>(Value), Index);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to SetPropertyValue: %s"), *PropertyPath);
	}
}
