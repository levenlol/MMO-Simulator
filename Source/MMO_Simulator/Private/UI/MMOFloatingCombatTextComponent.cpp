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

	// ...
	
}


// Called every frame
void UMMOFloatingCombatTextComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Get iewport size
	const FVector2D viewportSize = UWidgetLayoutLibrary::GetViewportSize(this);

	// check for bad viewport size
	if (viewportSize.X <= 0.0f || viewportSize.Y >= 0.0f)
		return;

	// Get viewport scale (DPI scale)
	const int32 viewportX = FGenericPlatformMath::FloorToInt(viewportSize.X);
	const int32 viewportY = FGenericPlatformMath::FloorToInt(viewportSize.Y);
	const float viewportScale = GetDefault<UUserInterfaceSettings>(UUserInterfaceSettings::StaticClass())->GetDPIScaleBasedOnSize(FIntPoint(viewportX, viewportY));

	// check for bad scale
	if (viewportScale <= 0.0f)
		return;

	// get the owning player's camera location
	const FVector cameraLocation = GetOwner<APawn>()->GetController<APlayerController>()->PlayerCameraManager->GetCameraLocation();

	// adjust for the vertical location of the active text actors by the viewport size and scale so that they appear evenly stacked on the screen
	for (int32 i = 1; i < ActiveTextActor.Num(); ++i)
	{
		// calculate the vertical offset based on how far we (the camera) are from the floating tect actor
		AMMOFloatingTextActor* textActor = ActiveTextActor[i];
		const float distance = FVector::Dist(cameraLocation, textActor->GetAnchorLocation());
		const float verticalOffset = distance / (viewportSize.X / viewportScale);

		// adjust the floating tect actor's location by the calculated amount
		textActor->SetActorLocation(textActor->GetAnchorLocation() + FVector(0.0f, 0.0f, i * verticalOffset * TextVerticalSpacing));

	}
}


void UMMOFloatingCombatTextComponent::AddFloatingText(const FText& text, FVector world_Location)
{
	if (FloatingTextActorClass == nullptr)
		return;

	const FTransform spawnTransform = FTransform(FRotator::ZeroRotator, world_Location + FVector(0.0f, 0.0f, TextVerticalOffset));
	AMMOFloatingTextActor* newTextActor = GetWorld()->SpawnActorDeferred<AMMOFloatingTextActor>(FloatingTextActorClass, spawnTransform, GetOwner());
	if (newTextActor == nullptr)
		return;

	// initialize and finish spawning the actor
	newTextActor->Initialize(text);
	newTextActor->OnDestroyed.AddDynamic(this, &UMMOFloatingCombatTextComponent::OnTextDestroyed);
	UGameplayStatics::FinishSpawningActor(newTextActor, spawnTransform);

	// add the new text actor to the beginning of the activeTextActor array
	ActiveTextActor.Insert(newTextActor, 0);
}


void UMMOFloatingCombatTextComponent::OnTextDestroyed(AActor* destroyed_actor)
{
	ActiveTextActor.Pop();
}