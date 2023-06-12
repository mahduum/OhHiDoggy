// Fill out your copyright notice in the Description page of Project Settings.


#include "OHDInputConfig.h"
#include "OhHiDoggy/OHDLogChannels.h"


UOHDInputConfig::UOHDInputConfig(const FObjectInitializer& ObjectInitializer)
{
}

const UInputAction* UOHDInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FDoggyInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogOHD, Error, TEXT("Can't find NativeInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}

const UInputAction* UOHDInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FDoggyInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	for (const FDoggyInputAction& Action : AbilityInputActionsWithInputData)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogOHD, Error, TEXT("Can't find AbilityInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}


void UOHDInputConfig::FindAbilityTagForInputAction(const UInputAction* InputAction, TOptional<FGameplayTag>& OutInputTag, bool bLogNotFound) const
{
	for (const FDoggyInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && (Action.InputAction == InputAction))
		{
			OutInputTag = Action.InputTag;
			break;
		}
	}

	for (const FDoggyInputAction& Action : AbilityInputActionsWithInputData)
	{
		if (Action.InputAction && (Action.InputAction == InputAction))
		{
			OutInputTag = Action.InputTag;
			break;
		}
	}

	if (bLogNotFound && OutInputTag.IsSet() == false)
	{
		UE_LOG(LogOHD, Error, TEXT("Can't find InputTag for AbilityInputAction with description: '[%s]' on InputConfig [%s]."), *InputAction->ActionDescription.ToString(), *GetNameSafe(this));
	}
}