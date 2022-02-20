// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/MMOBaseCharacter.h"
#include "Weapons/MMOBaseWeapon.h"

// Sets default values
AMMOBaseCharacter::AMMOBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMMOBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(RecuperateTimerHandle, this, &AMMOBaseCharacter::OnRecuperate, static_cast<float>(Stats.RecuperateEverySeconds), true, -1.f);
}

void AMMOBaseCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(RecuperateTimerHandle);
}

void AMMOBaseCharacter::OnRecuperate()
{
	Stats.Recuperate(Stats.RecuperateEverySeconds);
}

void AMMOBaseCharacter::DamageTake_Implementation(FMMODamage InDamage)
{

}

bool AMMOBaseCharacter::CanCharacterAttack() const
{
	// TODO
	return false;
}

bool AMMOBaseCharacter::TryEquipWeapon(TSubclassOf<AMMOBaseWeapon> InWeaponClass, bool bMainHand)
{
	if (!InWeaponClass)
		return false;

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.Instigator = this;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AMMOBaseWeapon* Weapon = GetWorld()->SpawnActor<AMMOBaseWeapon>(InWeaponClass, SpawnParameters);

	if (!Weapon)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn Weapon: %s"), *InWeaponClass->GetName());
		return false;
	}

	const bool bIs2H = UMMOGameplayUtils::Is2HWeapon(Weapon->WeaponType);
	if (MainHandWeapon && (bMainHand || bIs2H))
	{
		// TODO unequip main hand weapon
		MainHandWeapon->Destroy();
		MainHandWeapon = nullptr;
	}

	if (OffHandWeapon && (!bMainHand || bIs2H))
	{
		// TODO unequip off hand weapon
		OffHandWeapon->Destroy();
		OffHandWeapon = nullptr;
	}

	bMainHand = bMainHand || bIs2H; // 2h weapon should be put in the main hand

	// assign and attach new weapon
	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
	if (bMainHand || bIs2H)
	{
		MainHandWeapon = Weapon;
		Weapon->AttachToComponent(GetMesh(), AttachRules, FName("hand_r"));
	}
	else
	{
		OffHandWeapon = Weapon;
		Weapon->AttachToComponent(GetMesh(), AttachRules, FName("hand_l"));
	}

	return true;
}

// Called every frame
void AMMOBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
