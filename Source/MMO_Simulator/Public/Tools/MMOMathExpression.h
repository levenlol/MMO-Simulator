// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ThirdParty/tinyexpr.h"
#include "Tools/MMOFunctionLibrary.h"
#include "MMOMathExpression.generated.h"

class AMMOBaseCharacter;

USTRUCT(BlueprintType)
struct MMO_SIMULATOR_API FMMOMathExpression
{
	GENERATED_BODY()
public:
	FMMOMathExpression();
	~FMMOMathExpression();

	// Map of variable: VariableName -> PropertyPath.
	UPROPERTY(EditAnywhere, Category = MathIsMagic)
	TMap<FString, FString> VariableMap;

	// Description of expression
	UPROPERTY(EditAnywhere, Category = MathIsMagic)
	FString Expression;

	void Init(const UObject* InObject);
	bool IsValid() const { return ReferenceObject && compiledExpression; }

	void RefreshVariables();

	template<typename ValueType>
	ValueType Eval(bool bRefreshVariables = true);

	// release any allocated resource
	void Release();

private:
	UPROPERTY(VisibleAnywhere, Category = MathIsMagic)
	const UObject* ReferenceObject;

	// expression used to eval()
	te_expr* compiledExpression = nullptr;

	// cached reference to FProperty
	TArray<FMMOPropertyBlob> Blobs;

	// buffer containing the variables used for eval()
	UPROPERTY(VisibleAnywhere, Category = MathIsMagic)
	TArray<double> FoundVariables;

	TArray<char*> VariableNameCString;

	int32 LastError = 0;
	std::vector<te_variable> variablesMap;
};

template<typename ValueType>
FORCEINLINE ValueType FMMOMathExpression::Eval(bool bRefreshVariables)
{
	if (!IsValid())
	{
		UE_LOG(LogTemp, Fatal, TEXT("FMMOMathExpression::Eval() invoked with an invalid state."));
		return static_cast<ValueType>(0.0);
	}

	if (bRefreshVariables)
	{
		RefreshVariables();
	}
	
	return static_cast<ValueType>(te_eval(compiledExpression));
}
