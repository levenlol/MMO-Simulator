// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystem/Skills/MMOCameraShakeSkill.h"
#include "Camera/MMOPlayerCameraManager.h"

void UMMOCameraShakeSkill::Setup(AMMOBaseCharacter* InOwner)
{
	Super::Setup(InOwner);

	CameraManager = AMMOPlayerCameraManager::GetPlayerCameraManager(this, 0);
}

void UMMOCameraShakeSkill::CastAbility(const FMMOSkillInputData& Data)
{
	Super::CastAbility(Data);

	if (CameraShake && CameraManager)
	{
		CameraManager->StartCameraShake(CameraShake, 1.f, ECameraShakePlaySpace::CameraLocal);
	}
}
