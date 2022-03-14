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
		ParsePropertyForIndex(Properties[i], PropertyName, PropertyArrayIndex);

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
	ParsePropertyForIndex(Properties.Last(), PropertyName, PropertyArrayIndex);
	OutIndex = PropertyArrayIndex;

	return FindFProperty<FProperty>(Struct, *Properties.Last());
}

FMMOPropertyBlob UMMOFunctionLibrary::FindProperty(const UObject* InObject, const FString& PropertyPath)
{
	FMMOPropertyBlob Blob;
	Blob.Path = PropertyPath;
	Blob.Property = UMMOFunctionLibrary::FindProperty(InObject, PropertyPath, Blob.Leaf, Blob.Index);

	return Blob;
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

TArray<FMMOPropertyBlob> UMMOFunctionLibrary::ListProperties(const UObject* InObject, const TArray<FName>& ClassFilters)
{
	if (!InObject)
		return {};

	return ListProperties(InObject->GetClass(), ClassFilters);
}

TArray<FMMOPropertyBlob> UMMOFunctionLibrary::ListProperties(UStruct* InClass, const TArray<FName>& ClassFilters)
{
	TArray<FMMOPropertyBlob> Blobs;

	ListProperties_Recursive(InClass, "", Blobs, ClassFilters);

	return Blobs;
}

FMMOPropertyBlob UMMOFunctionLibrary::FindPropertyOfType(UStruct* InClass, const FString& PropertyType)
{
	for (TFieldIterator<FProperty> PropIt(InClass); PropIt; ++PropIt)
	{
		FProperty* Property = *PropIt;

		if (FStructProperty* StructProp = CastField<FStructProperty>(Property))
		{
			if (StructProp->Struct->GetName() == PropertyType)
			{
				FMMOPropertyBlob Blob;
				Blob.Index = 0;
				Blob.Leaf = nullptr;
				Blob.Path = Property->GetName();
				Blob.Property = Property;

				return Blob;
			}
		}
		
	}

	return FMMOPropertyBlob();
}

TArray<FMMOPropertyBlob> UMMOFunctionLibrary::FindPropertiesOfType(UStruct* InClass, const FString& PropertyType)
{
	TArray<FMMOPropertyBlob> Blobs;
	for (TFieldIterator<FProperty> PropIt(InClass); PropIt; ++PropIt)
	{
		FProperty* Property = *PropIt;

		if (FStructProperty* StructProp = CastField<FStructProperty>(Property))
		{
			if (StructProp->Struct->GetName() == PropertyType)
			{
				FMMOPropertyBlob Blob;
				Blob.Index = 0;
				Blob.Leaf = nullptr;
				Blob.Path = Property->GetName();
				Blob.Property = Property;

				Blobs.Add(Blob);
			}
		}

	}

	return Blobs;
}

void UMMOFunctionLibrary::ListProperties_Recursive(UStruct* Struct, FString Path, TArray<FMMOPropertyBlob>& OutProperties, const TArray<FName>& ClassFilters)
{
	// basically if we have a case where a name is repeated in the path, we consider it a loop.
	// that is not 100% right but is close enough for us (we should also check the type to make it better).
	static auto CheckLoop = [](const FString& Path, const FString& CurrentPropName)
	{
		TArray<FString> Array;
		Path.ParseIntoArray(Array, TEXT("."));

		return Array.Contains(CurrentPropName);
	};

	for (TFieldIterator<FProperty> PropIt(Struct); PropIt; ++PropIt)
	{
		FProperty* Property = *PropIt;
		
		// filter out actor & object properties as we dont need them.
		if (ClassFilters.Contains(*Property->Owner.GetName()) || CheckLoop(Path, Property->GetName()))
		{
			continue;
		}

		// First Handle Structured properties (array, struct and objects).
		if (FStructProperty* StructProperty = CastField<FStructProperty>(Property))
		{
			Struct = StructProperty->Struct;
			ListProperties_Recursive(Struct, Path.IsEmpty() ? StructProperty->GetName() : Path + TEXT(".") + StructProperty->GetName(), OutProperties, ClassFilters);
		}
		else if (FObjectProperty* ObjProperty = CastField<FObjectProperty>(Property))
		{
			Struct = ObjProperty->PropertyClass;
			
			ListProperties_Recursive(Struct, Path.IsEmpty() ? ObjProperty->GetName() : Path + TEXT(".") + ObjProperty->GetName(), OutProperties, ClassFilters);
		}
		else if (FArrayProperty* ArrayProp = CastField<FArrayProperty>(Property))
		{

		}
		else if (CastField<FNumericProperty>(Property))
		{
			// leaf property
			FString TempPath = Path;
			if (!TempPath.IsEmpty())
			{
				TempPath += TEXT(".");
			}
			TempPath += Property->GetName();

			FMMOPropertyBlob Blob;
			Blob.Index = 0;
			Blob.Leaf = nullptr; // we are just listing all the properties.
			Blob.Path = TempPath;
			Blob.Property = Property;

			OutProperties.Add(Blob);
		}
	}
}

void UMMOFunctionLibrary::ParsePropertyForIndex(const FString& InProperty, FString& OutName, int32& OutIndex)
{
	if (InProperty.Len() > 0 && InProperty.GetCharArray()[InProperty.Len() - 1] == ']')
	{
		int32 OpenIndex = 0;
		if (InProperty.FindLastChar('[', OpenIndex))
		{
			FString TruncatedPropertyName(OpenIndex, *InProperty);
			OutName = TruncatedPropertyName;

			const int32 NumberLength = InProperty.Len() - OpenIndex - 2;
			if (NumberLength > 0 && NumberLength <= 10)
			{
				TCHAR NumberBuffer[11];
				FMemory::Memcpy(NumberBuffer, &InProperty[OpenIndex + 1], sizeof(TCHAR) * NumberLength);
				LexFromString(OutIndex, NumberBuffer);
			}

			return;
		}
	}

	OutName = InProperty;
	OutIndex = 0;
}
