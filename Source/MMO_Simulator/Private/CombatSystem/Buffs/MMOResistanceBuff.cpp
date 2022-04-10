// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystem/Buffs/MMOResistanceBuff.h"

void UMMOResistanceBuff::ProcessDamageTaken(FMMODamage& InOutDamage)
{
	if (InOutDamage.Damage > 0 && ResistanceMap.Contains(InOutDamage.DamageType))
	{
		InOutDamage.Damage -= static_cast<int32>(InOutDamage.Damage * (ResistanceMap[InOutDamage.DamageType] * 0.01f));
	}
}
