// Fill out your copyright notice in the Description page of Project Settings.

#include "OHDGameplayAbility_TurnInPlace90.h"
#include "AbilitySystemComponent.h"
#include "OhHiDoggy/FOHDGameplayTags.h"
#include "OhHiDoggy/OHDLogChannels.h"
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
		UE_LOG(LogOHD, Warning, TEXT("Cannot activate turn in place ability 1."));
		return false;
	}

	//if(this->GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(AbilityTag));)

	const ACanineCharacter* OHDCharacter = Cast<ACanineCharacter>(ActorInfo->AvatarActor.Get());
	if (!OHDCharacter || !OHDCharacter->CanTurnInPlace90())
	{
		UE_LOG(LogOHD, Warning, TEXT("Cannot activate turn in place ability 2 [%i], [%i]."), !OHDCharacter, !OHDCharacter->CanTurnInPlace90());
		return false;
	}

	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		UE_LOG(LogOHD, Warning, TEXT("Cannot activate turn in place ability 3."));
		return false;
	}

	UE_LOG(LogOHD, Warning, TEXT("Can activate turn in place ability check, can turn in place: %i, pressed turn in place: %i"), OHDCharacter->CanTurnInPlace90(), OHDCharacter->bPressedTurnInPlace90);

	return true;
}

void UOHDGameplayAbility_TurnInPlace90::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	//ActivationOwnedTags.Reset();
	// Stop TurnInPlace90ing in case the ability blueprint doesn't call it.

	CharacterTurnInPlace90Stop();
	//const ACanineCharacter* OHDCharacter = Cast<ACanineCharacter>(ActorInfo->AvatarActor.Get());
	//UE_LOG(LogOHD, Warning, TEXT("End ability, can turn in place: %i, pressed turn in place: %i"), OHDCharacter->CanTurnInPlace90(), OHDCharacter->bPressedTurnInPlace90);
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);//todo sometimes this ability is not properly ended!!!
}

void UOHDGameplayAbility_TurnInPlace90::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogOHD, Display, TEXT("Turn in place ability activated (also to check whether it was activated by tag."));
	CharacterTurnInPlace90Start();
	if(const UAbilityTurnInPlace90InputData* const InputData = Cast<UAbilityTurnInPlace90InputData>(
		TriggerEventData->OptionalObject))
	{
		const double XValue = InputData->StartAbilityInputActionValue.Get<FVector2D>().X;
		
		if(FMath::IsNearlyEqual(XValue, 0) == false)
		{
			// TODO note: tags left/right should be added before this activity is activated?
			const FGameplayTag TagToAdd = XValue > 0
				                              ? FOHDGameplayTags::Get().InputTag_TurnRight90
				                              : FOHDGameplayTags::Get().InputTag_TurnLeft90;
			
			//GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(TagToAdd);//todo make it current tag to remove on end

			UE_LOG(LogOHD, Display, TEXT("Turn in place ability has added tag: %s."), *TagToAdd.GetTagName().ToString());//todo why it can be added only once?
		}
	}
	//todo when binding it pass input action data? is it with TryActivateAbility?
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UOHDGameplayAbility_TurnInPlace90::SendGameplayEvent(FGameplayTag EventTag, FGameplayEventData Payload)//todo how to use it? for example send the event to the receiver of the ability's side effects
{
	Super::SendGameplayEvent(EventTag, Payload);
}

void UOHDGameplayAbility_TurnInPlace90::CharacterTurnInPlace90Start()//todo make BP 
{
	if (ACanineCharacter* OHDCharacter = GetOHDCharacterFromActorInfo())
	{
		if (OHDCharacter->IsLocallyControlled() && !OHDCharacter->bPressedTurnInPlace90)
		{
			OHDCharacter->UnCrouch();
			OHDCharacter->TurnInPlace90(); 
			UE_LOG(LogOHD, Warning, TEXT("Set turn in place start, can turn in place: %i, pressed turn in place: %i"), OHDCharacter->CanTurnInPlace90(), OHDCharacter->bPressedTurnInPlace90);

		}
	}
}

void UOHDGameplayAbility_TurnInPlace90::CharacterTurnInPlace90Stop()//todo make BP
{
	if (ACanineCharacter* OHDCharacter = GetOHDCharacterFromActorInfo())
	{
		if (OHDCharacter->IsLocallyControlled() && OHDCharacter->bPressedTurnInPlace90)
		{
			OHDCharacter->StopTurningInPlace90();
			UE_LOG(LogOHD, Warning, TEXT("Set turn in place stop, can turn in place: %i, pressed turn in place: %i"), OHDCharacter->CanTurnInPlace90(), OHDCharacter->bPressedTurnInPlace90);
		}
	}
}
