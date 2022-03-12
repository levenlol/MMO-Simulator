// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Tools/MMOMathExpression.h"
#include "MathExpressionUtilityWidget.generated.h"

class UButton;
class UDetailsView;

UCLASS(Blueprintable, BlueprintType)
class UMathExpressionUtilityWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Asset)
	UBlueprint* AssetPtr;

	UPROPERTY(meta = (BindWidget))
	UDetailsView* ExpressionsView;
	
	UPROPERTY(meta = (BindWidget))
	UButton* LoadConfigButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ImportButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ClearMapButton;
	
	UFUNCTION(BlueprintImplementableEvent, Category = Widget)
	void AddPropertyData(const FString& Path);

	UFUNCTION(BlueprintImplementableEvent, Category = Widget)
	void Clear();

	UFUNCTION(BlueprintImplementableEvent, Category = Widget)
	void GetSelectedProperties(TArray<FString>& Names, TArray<FString>& Paths);

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UFUNCTION()
	void OnLoadConfig();

	UFUNCTION()
	void OnImportToMap();

	UFUNCTION()
	void OnClearMap();
};
