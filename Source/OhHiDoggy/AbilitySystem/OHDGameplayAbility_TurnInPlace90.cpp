// Fill out your copyright notice in the Description page of Project Settings.


#include "OHDGameplayAbility_TurnInPlace90.h"

#include "OhHiDoggy/CanineCharacter/CanineCharacter.h"

UOHDGameplayAbility_TurnInPlace90::UOHDGameplayAbility_TurnInPlace90(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool UOHDGameplayAbility_TurnInPlace90::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}

	const ACanineCharacter* OHDCharacter = Cast<ACanineCharacter>(ActorInfo->AvatarActor.Get());
	if (!OHDCharacter || !OHDCharacter->CanTurnInPlace90())
	{
		return false;
	}

	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return true;
}

void UOHDGameplayAbility_TurnInPlace90::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// Stop TurnInPlace90ing in case the ability blueprint doesn't call it.
	CharacterTurnInPlace90Stop();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UOHDGameplayAbility_TurnInPlace90::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	//todo when binding it pass input action data? is it with TryActivateAbility?
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UOHDGameplayAbility_TurnInPlace90::CharacterTurnInPlace90Start()
{
	if (ACanineCharacter* OHDCharacter = GetOHDCharacterFromActorInfo())
	{
		if (OHDCharacter->IsLocallyControlled() && !OHDCharacter->bPressedTurnInPlace90)
		{
			OHDCharacter->UnCrouch();
			OHDCharacter->TurnInPlace90();
		}
	}
}

void UOHDGameplayAbility_TurnInPlace90::CharacterTurnInPlace90Stop()
{
	if (ACanineCharacter* OHDCharacter = GetOHDCharacterFromActorInfo())
	{
		if (OHDCharacter->IsLocallyControlled() && OHDCharacter->bPressedTurnInPlace90)
		{
			OHDCharacter->StopTurningInPlace90();
		}
	}
}
