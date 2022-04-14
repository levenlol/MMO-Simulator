// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MMOCrowdAIController.h"
#include "Navigation/CrowdFollowingComponent.h"


AMMOCrowdAIController::AMMOCrowdAIController(const FObjectInitializer& ObjInit)
	: Super(ObjInit.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
}
