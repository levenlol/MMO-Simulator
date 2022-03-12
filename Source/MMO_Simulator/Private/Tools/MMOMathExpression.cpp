// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/MMOMathExpression.h"
#include "Tools/MMOFunctionLibrary.h"

FMMOMathExpression::FMMOMathExpression()
{
}

FMMOMathExpression::~FMMOMathExpression()
{
	Release();
}

double FMMOPropertyBlob::GetValue() const
{
	if (!Property)
	{
		return 0.0;
	}

	if (Property->IsA<FIntProperty>())
	{
		return static_cast<double>(*GetValue<int32>());
	}
	else if (Property->IsA<FFloatProperty>())
	{
		return static_cast<double>(*GetValue<float>());
	}
	else if (Property->IsA<FDoubleProperty>())
	{
		return *GetValue<double>();
	}

	return 0.0;
}


void FMMOMathExpression::Init(const UObject* InObject)
{
	if (!InObject)
	{
		UE_LOG(LogTemp, Error, TEXT("FMMOMathExpression: Null object."));
		return;
	}

	if (Expression.IsEmpty())
	{
		UE_LOG(LogTemp, Verbose, TEXT("FMMOMathExpression: expression isn't set."));
		return;
	}

	Release();

	ReferenceObject = InObject;

	const int32 MapsNum = VariableMap.Num();

	variablesMap.reserve(MapsNum);
	FoundVariables.SetNum(MapsNum);

	TArray<FString> Keys;
	VariableMap.GenerateKeyArray(Keys);
	for (int32 i = 0; i < MapsNum; i++)
	{
		const FString& Key = Keys[i];
		const FString& Value = VariableMap[Key];

		if (Key.Len() > 31)
		{
			UE_LOG(LogTemp, Fatal, TEXT("VariableNames MUST be less than 32 char."));
		}

		if (Key.IsEmpty() || Value.IsEmpty())
			continue;

		// Find the property we want to track by reflection.
		FMMOPropertyBlob Blob = UMMOFunctionLibrary::FindProperty(ReferenceObject, Value);
		if (!Blob.Property->IsA<FNumericProperty>())
		{
			UE_LOG(LogTemp, Warning, TEXT("FMMOMathExpression: %s only numerical uproperties are supported."), *Value);
			continue;
		}
		
		Blobs.Add(Blob);

		// allocate and initialize variable container. (double is used to store values).
		const double value = Blob.GetValue();
		FoundVariables[i] = value;

		// Allocate variable name.
		std::string s(TCHAR_TO_ANSI(*Key), Key.Len());

		char* name = new char[32];
		memcpy(name, s.c_str(), s.size() + 1);
		
		VariableNameCString.Add(name);

		// create te_variable (only need name and pointer to memory).
		te_variable variable;
		variable.name = name;
		variable.address = &FoundVariables[i];
		variable.type = 0;
		variable.context = 0x0;

		variablesMap.push_back(variable);
	}

	auto ansiExpression = TCHAR_TO_ANSI(*Expression);
	compiledExpression = te_compile(ansiExpression, variablesMap.data(), variablesMap.size(), &LastError);
	ensure(compiledExpression);

	if (!compiledExpression)
	{
		UE_LOG(LogTemp, Error, TEXT("FMMOMathExpression: Error Compiling Expression: %s."), *Expression);
	}
}

void FMMOMathExpression::RefreshVariables()
{
	if (!IsValid())
		return;

	ensure(FoundVariables.Num() == Blobs.Num());
	for (int32 i = 0; i < FoundVariables.Num(); i++)
	{
		const FMMOPropertyBlob& Blob = Blobs[i];
		FoundVariables[i] = Blob.GetValue();
	}
}

void FMMOMathExpression::Release()
{
	ReferenceObject = nullptr;
	LastError = 0;

	variablesMap.clear();

	Blobs.Empty();
	FoundVariables.Empty();

	for (char* name : VariableNameCString)
	{
		delete[] name;
	}
	VariableNameCString.Empty();

	if (compiledExpression)
	{
		te_free(compiledExpression);
		compiledExpression = nullptr;
	}
}
