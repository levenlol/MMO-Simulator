// Fill out your copyright notice in the Description page of Project Settings.


#include "Buildings/MMOPreviewBuilding.h"
#include "Components/StaticMeshComponent.h"

void AMMOPreviewBuilding::SetPreviewMesh(UStaticMesh* InMesh)
{
	if (!InMesh)
		return;

	Mesh->SetStaticMesh(InMesh);
	
	for (int32 i = 0; i < Mesh->GetNumMaterials(); i++)
	{
		Mesh->SetMaterial(i, PreviewMaterial);
	}

	ChangeMeshColor();
}

bool AMMOPreviewBuilding::IsColliding() const
{
	TArray<AActor*> OverlapActors;
	Mesh->GetOverlappingActors(OverlapActors);
	
	return OverlapActors.Num() > 0;
}

void AMMOPreviewBuilding::ChangeMeshColor()
{
	if (!IsColliding())
	{
		Mesh->SetVectorParameterValueOnMaterials(FName("Color"), FVector(0.f, 1.f, 0.f));
	}
	else
	{
		Mesh->SetVectorParameterValueOnMaterials(FName("Color"), FVector(1.f, 0.f, 0.f));
	}
}
