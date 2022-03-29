// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MMOFloatingCombatTextComponent.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Runtime/Engine/Classes/Engine/UserInterfaceSettings.h"
#include "UI/MMOFloatingTextActor.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UMMOFloatingCombatTextComponent::UMMOFloatingCombatTextComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMMOFloatingCombatTextComponent::BeginPlay()
{
	Super::BeginPlay();

	InitPool();

	PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
}


// Called every frame
void UMMOFloatingCombatTextComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Get iewport size
	const FVector2D viewportSize = UWidgetLayoutLibrary::GetViewportSize(this);

	// check for bad viewport size
	if (viewportSize.X <= 0.0f || viewportSize.Y <= 0.0f || !PlayerCameraManager)
		return;

	// Get viewport scale (DPI scale)
	const int32 viewportX = FGenericPlatformMath::FloorToInt(viewportSize.X);
	const int32 viewportY = FGenericPlatformMath::FloorToInt(viewportSize.Y);
	const float viewportScale = GetDefault<UUserInterfaceSettings>(UUserInterfaceSettings::StaticClass())->GetDPIScaleBasedOnSize(FIntPoint(viewportX, viewportY));

	// check for bad scale
	if (viewportScale <= 0.0f)
		return;

	// get the owning player's camera location
	const FVector cameraLocation = PlayerCameraManager->GetCameraLocation();

	// adjust for the vertical location of the active text actors by the viewport size and scale so that they appear evenly stacked on the screen
	for (int32 i = 0; i < ActiveTextActors.Num(); ++i)
	{
		// calculate the vertical offset based on how far we (the camera) are from the floating tect actor
		AMMOFloatingTextActor* textActor = ActiveTextActors[i];
		const float distance = FVector::Dist(cameraLocation, textActor->GetAnchorLocation());
		const float verticalOffset = distance / (viewportSize.X / viewportScale);

		// adjust the floating tect actor's location by the calculated amount
		textActor->SetActorLocation(textActor->GetAnchorLocation() + FVector(0.0f, 0.0f, i * verticalOffset * TextVerticalSpacing));
	}
}


void UMMOFloatingCombatTextComponent::AddFloatingText(const FText& Text, const FVector& WorldLocation)
{
	if (FloatingTextActorClass == nullptr)
		return;

	AMMOFloatingTextActor* TextActor = TextActorPool.Num() > 0 ? TextActorPool.Pop() : nullptr;
	if (TextActor == nullptr)
		return;

	SetTextActorActive(TextActor, true);

	// initialize and finish spawning the actor
	TextActor->Initialize(Text);

	const FVector AnchorLocation = WorldLocation + FVector(0.0f, 0.0f, TextVerticalOffset);
	TextActor->SetActorLocation(AnchorLocation);
	TextActor->SetAnchorLocation(AnchorLocation);

	ActiveTextActors.Add(TextActor);
}

void UMMOFloatingCombatTextComponent::SetTextActorActive(AMMOFloatingTextActor* InActor, const bool bActive)
{
	if (!InActor)
		return;

	InActor->SetActorEnableCollision(false); // never allow collision with this actor.
	InActor->SetActorHiddenInGame(!bActive);
}

void UMMOFloatingCombatTextComponent::InitPool()
{
	if (!FloatingTextActorClass)
		return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.bNoFail = true;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ActiveTextActors.Reserve(PoolSize);
	TextActorPool.Reserve(PoolSize);

	for (int32 i = 0; i < PoolSize; i++)
	{
		AMMOFloatingTextActor* SpawnedActor = GetWorld()->SpawnActor<AMMOFloatingTextActor>(FloatingTextActorClass, FTransform::Identity, SpawnParams);
		SpawnedActor->OnAnimationFinished.AddDynamic(this, &UMMOFloatingCombatTextComponent::Recycle);
		
		SetTextActorActive(SpawnedActor, false);
		TextActorPool.Add(SpawnedActor);
	}
}


void UMMOFloatingCombatTextComponent::Recycle(AMMOFloatingTextActor* Sender)
{
	if (Sender)
	{
		SetTextActorActive(Sender, false);
		TextActorPool.Add(Sender);
		ActiveTextActors.Remove(Sender);
	}
}