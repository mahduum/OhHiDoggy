// Fill out your copyright notice in the Description page of Project Settings.


#include "OHDGameplayEffectContext.h"
#include "Components/PrimitiveComponent.h"
#include "OHDAbilitySourceInterface.h"

FOHDGameplayEffectContext* FOHDGameplayEffectContext::ExtractEffectContext(struct FGameplayEffectContextHandle Handle)
{
	FGameplayEffectContext* BaseEffectContext = Handle.Get();
	if ((BaseEffectContext != nullptr) && BaseEffectContext->GetScriptStruct()->IsChildOf(FOHDGameplayEffectContext::StaticStruct()))
	{
		return (FOHDGameplayEffectContext*)BaseEffectContext;
	}

	return nullptr;
}

bool FOHDGameplayEffectContext::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	FGameplayEffectContext::NetSerialize(Ar, Map, bOutSuccess);

	// Not serialized for post-activation use:
	// CartridgeID

	return true;
}

void FOHDGameplayEffectContext::SetAbilitySource(const IOHDAbilitySourceInterface* InObject, float InSourceLevel)
{
	AbilitySourceObject = MakeWeakObjectPtr(Cast<const UObject>(InObject));
	//SourceLevel = InSourceLevel;
}

const IOHDAbilitySourceInterface* FOHDGameplayEffectContext::GetAbilitySource() const
{
	return Cast<IOHDAbilitySourceInterface>(AbilitySourceObject.Get());
}

const UPhysicalMaterial* FOHDGameplayEffectContext::GetPhysicalMaterial() const
{
	if (const FHitResult* HitResultPtr = GetHitResult())
	{
		return HitResultPtr->PhysMaterial.Get();
	}
	return nullptr;
}