// Fill out your copyright notice in the Description page of Project Settings.


#include "OhHiDoggyInputComponent.h"


UOhHiDoggyInputComponent::UOhHiDoggyInputComponent(const FObjectInitializer& ObjectInitializer)
{
}

void UOhHiDoggyInputComponent::AddInputMappings(const UDoggyInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	//UOhHiDoggyLocalPlayer* LocalPlayer = InputSubsystem->GetLocalPlayer<UOhHiDoggyLocalPlayer>();//todo add local player
	//check(LocalPlayer);

	// Add any registered input mappings from the settings!
	// if (UOhHiDoggySettingsLocal* LocalSettings = UOhHiDoggySettingsLocal::Get())//todo add custom settings
	// {
	// 	// We don't want to ignore keys that were "Down" when we add the mapping context
	// 	// This allows you to die holding a movement key, keep holding while waiting for respawn,
	// 	// and have it be applied after you respawn immediately. Leaving bIgnoreAllPressedKeysUntilRelease
	// 	// to it's default "true" state would require the player to release the movement key,
	// 	// and press it again when they respawn
	// 	FModifyContextOptions Options = {};
	// 	Options.bIgnoreAllPressedKeysUntilRelease = false;
	// 	
	// 	// Add all registered configs, which will add every input mapping context that is in it
	// 	const TArray<FLoadedMappableConfigPair>& Configs = LocalSettings->GetAllRegisteredInputConfigs();//todo add mappable
	// 	for (const FLoadedMappableConfigPair& Pair : Configs)
	// 	{
	// 		if (Pair.bIsActive)
	// 		{
	// 			InputSubsystem->AddPlayerMappableConfig(Pair.Config, Options);	
	// 		}
	// 	}
	// 	
	// 	// Tell enhanced input about any custom keymappings that we have set
	// 	for (const TPair<FName, FKey>& Pair : LocalSettings->GetCustomPlayerInputConfig())
	// 	{
	// 		if (Pair.Key != NAME_None && Pair.Value.IsValid())
	// 		{
	// 			InputSubsystem->AddPlayerMappedKey(Pair.Key, Pair.Value);
	// 		}
	// 	}
	//}
}

void UOhHiDoggyInputComponent::RemoveInputMappings(const UDoggyInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	// UOhHiDoggyLocalPlayer* LocalPlayer = InputSubsystem->GetLocalPlayer<UOhHiDoggyLocalPlayer>();
	// check(LocalPlayer);
	//
	// if (UOhHiDoggySettingsLocal* LocalSettings = UOhHiDoggySettingsLocal::Get())
	// {
	// 	// Remove any registered input contexts
	// 	const TArray<FLoadedMappableConfigPair>& Configs = LocalSettings->GetAllRegisteredInputConfigs();
	// 	for (const FLoadedMappableConfigPair& Pair : Configs)
	// 	{
	// 		InputSubsystem->RemovePlayerMappableConfig(Pair.Config);
	// 	}
	// 	
	// 	// Clear any player mapped keys from enhanced input
	// 	for (const TPair<FName, FKey>& Pair : LocalSettings->GetCustomPlayerInputConfig())
	// 	{
	// 		InputSubsystem->RemovePlayerMappedKey(Pair.Key);
	// 	}
	// }
}

void UOhHiDoggyInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}

// void UOhHiDoggyInputComponent::AddInputConfig(const FLoadedMappableConfigPair& ConfigPair, UEnhancedInputLocalPlayerSubsystem* InputSubsystem)
// {
// 	check(InputSubsystem);
// 	if (ensure(ConfigPair.bIsActive))
// 	{
// 		InputSubsystem->AddPlayerMappableConfig(ConfigPair.Config);	
// 	}
// }
//
// void UOhHiDoggyInputComponent::RemoveInputConfig(const FLoadedMappableConfigPair& ConfigPair, UEnhancedInputLocalPlayerSubsystem* InputSubsystem)
// {
// 	check(InputSubsystem);
// 	if (!ConfigPair.bIsActive)
// 	{
// 		InputSubsystem->AddPlayerMappableConfig(ConfigPair.Config);	
// 	}	
// }