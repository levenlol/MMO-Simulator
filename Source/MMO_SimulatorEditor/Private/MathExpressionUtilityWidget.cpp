// Fill out your copyright notice in the Description page of Project Settings.


#include "MathExpressionUtilityWidget.h"
#include "Components/Button.h"
#include "Tools/MMOFunctionLibrary.h"
#include "Components/DetailsView.h"


void UMathExpressionUtilityWidget::NativeConstruct()
{
	Super::NativeConstruct();

	LoadConfigButton->OnClicked.AddDynamic(this, &UMathExpressionUtilityWidget::OnLoadConfig);
	ImportButton->OnClicked.AddDynamic(this, &UMathExpressionUtilityWidget::OnImportToMap);
	ClearMapButton->OnClicked.AddDynamic(this, &UMathExpressionUtilityWidget::OnClearMap);
}

void UMathExpressionUtilityWidget::NativeDestruct()
{
	Super::NativeDestruct();
	

	LoadConfigButton->OnClicked.RemoveDynamic(this, &UMathExpressionUtilityWidget::OnLoadConfig);
	ImportButton->OnClicked.RemoveDynamic(this, &UMathExpressionUtilityWidget::OnImportToMap);
	ClearMapButton->OnClicked.RemoveDynamic(this, &UMathExpressionUtilityWidget::OnClearMap);
}

void UMathExpressionUtilityWidget::OnLoadConfig()
{
	Clear();

	if (!AssetPtr)
	{
		return;
	}

	TArray<FName> ClassFilters = { "Actor", "Object", "Pawn", "Character", "ActorComponent", 
		"WidgetComponent", "LocalPlayer", "GameViewportClient", "Console", "PlayerCameraManager",
		"StaticMeshComponent", "SkeletalMeshComponent", "MeshComponent",
		"PrimitiveComponent", "SceneComponent", "DecalComponent"
	};

	TArray<FMMOPropertyBlob> OutProperties = UMMOFunctionLibrary::ListProperties(*AssetPtr->GeneratedClass, ClassFilters);

	OutProperties.Sort([](const FMMOPropertyBlob& First, const FMMOPropertyBlob& Second)
		{
			TArray<FString> FirstArray;
			TArray<FString> SecondArray;

			First.Path.ParseIntoArray(FirstArray, TEXT("."));
			Second.Path.ParseIntoArray(SecondArray, TEXT("."));

			if (FirstArray.Num() != SecondArray.Num())
			{
				return FirstArray.Num() < SecondArray.Num();
			}

			return First.Path < Second.Path;
		});

	for (const FMMOPropertyBlob& Blob : OutProperties)
	{
		AddPropertyData(Blob.Path);
	}

	TArray<FMMOPropertyBlob> Blobs = UMMOFunctionLibrary::FindPropertiesOfType(*AssetPtr->GeneratedClass, TEXT("MMOMathExpression"));

	ExpressionsView->PropertiesToShow.Empty();
	for (const FMMOPropertyBlob& Blob : Blobs)
	{
		ExpressionsView->PropertiesToShow.Add(*Blob.Path);
	}
	ExpressionsView->SetObject(Blobs.Num() > 0 ? AssetPtr->GeneratedClass->GetDefaultObject() : nullptr);

	ensure(OutProperties.Num() > 0); 
}

void UMathExpressionUtilityWidget::OnImportToMap()
{
	if (!AssetPtr)
	{
		return;
	}

	TArray<FString> Names;
	TArray<FString> Paths;
	GetSelectedProperties(Names, Paths);

	check(Names.Num() == Paths.Num() && "Received a different num of Names and Paths");

	UObject* DefaultObject = AssetPtr->GeneratedClass->GetDefaultObject();

	for (const FName& Name : ExpressionsView->PropertiesToShow)
	{
		FMMOMathExpression* Expr = UMMOFunctionLibrary::GetPropertyValue<FMMOMathExpression>(DefaultObject, Name.ToString());

		if (Expr)
		{
			Expr->VariableMap.Empty();
			for (int32 i = 0; i < Names.Num(); i++)
			{
				Expr->VariableMap.Add(Names[i], Paths[i]);
			}
		}
		
	}
}

void UMathExpressionUtilityWidget::OnClearMap()
{
	if (!AssetPtr)
	{
		return;
	}

	UObject* DefaultObject = AssetPtr->GeneratedClass->GetDefaultObject();

	for (const FName& Name : ExpressionsView->PropertiesToShow)
	{
		FMMOMathExpression* Expr = UMMOFunctionLibrary::GetPropertyValue<FMMOMathExpression>(DefaultObject, Name.ToString());

		if (Expr)
		{
			TArray<FString> KeyArray;
			Expr->VariableMap.GenerateKeyArray(KeyArray);

			// find not needed keys.
			KeyArray = KeyArray.FilterByPredicate([Expression = Expr->Expression](const FString& Str) { return !Expression.Contains(Str); });

			for (const FString& KeyToRemove : KeyArray)
			{
				Expr->VariableMap.Remove(KeyToRemove);
			}
		}

	}
}
