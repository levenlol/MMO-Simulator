// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/MMODataFinder.h"
#include "Misc/MessageDialog.h"
#include "Engine/DataTable.h"

UMMODataFinder* UMMODataFinder::Instance{ nullptr };

void UMMODataFinder::Startup(UObject* Outer)
{
	check(!Instance);
	Instance = NewObject<UMMODataFinder>(Outer);
	Instance->AddToRoot();
	Instance->Init();
}

void UMMODataFinder::Shutdown()
{
	check(Instance);
	Instance->Uninit();
	Instance->RemoveFromRoot();
	Instance = nullptr;
}

UAnimSequenceBase* UMMODataFinder::GetAnimSequence(const FMMOWeaponTypeCouple& WeaponCouple) const
{
	if (AnimationsMap.Contains(WeaponCouple))
	{
		return AnimationsMap[WeaponCouple];
	}

	return nullptr;
}

void UMMODataFinder::Init()
{
	ParseAnimationDataTable();
}

void UMMODataFinder::Uninit()
{
	AnimationsMap.Empty();
}

void UMMODataFinder::ParseAnimationDataTable()
{
	UMMOGameInstance* GameInstance = Cast<UMMOGameInstance>(GetOuter());
	UDataTable* AnimationDataTable = GameInstance->RetrieveDataTable(AnimationDataTableName);

	if (!AnimationDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("DataFinder: cannot find AnimationDatatable"));
		return;
	}

	TArray<FMMOAnimationDataTable*> AllRows;
	AnimationDataTable->GetAllRows<FMMOAnimationDataTable>(TEXT("DATA_RETRIEVER"), AllRows);

	for (FMMOAnimationDataTable* Row : AllRows)
	{
		const FMMOWeaponTypeCouple& WeaponType = Row->WeaponsType;
#if WITH_EDITORONLY_DATA
		if (AnimationsMap.Contains(WeaponType) || !Row->WeaponAnimation)
		{
			const FText Title = FText::FromString(TEXT("Ciavola Fixa"));
			FMessageDialog::Open(EAppMsgType::Type::Ok, FText::FromString(TEXT("Found Duplicated Row or Null animation")), &Title);
		}
		else
#endif
		{
			AnimationsMap.Add(WeaponType, Row->WeaponAnimation);
		}
	}
}
