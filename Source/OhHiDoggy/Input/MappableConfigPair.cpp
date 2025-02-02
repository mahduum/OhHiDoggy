﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "MappableConfigPair.h"
#include "ICommonUIModule.h"
#include "CommonUISettings.h"
#include "OhHiDoggy/Settings/OHDSettingsLocal.h"
#include "OhHiDoggy/System/OHDAssetManager.h"

bool FMappableConfigPair::CanBeActivated() const
{
	const FGameplayTagContainer& PlatformTraits = ICommonUIModule::GetSettings().GetPlatformTraits();

	// If the current platform does NOT have all the dependent traits, then don't activate it
	if (!DependentPlatformTraits.IsEmpty() && !PlatformTraits.HasAll(DependentPlatformTraits))
	{
		return false;
	}

	// If the platform has any of the excluded traits, then we shouldn't activate this config.
	if (!ExcludedPlatformTraits.IsEmpty() && PlatformTraits.HasAny(ExcludedPlatformTraits))
	{
		return false;
	}

	return true;
}

bool FMappableConfigPair::RegisterPair(const FMappableConfigPair& Pair)
{
	UOHDAssetManager& AssetManager = UOHDAssetManager::Get();//todo primary register pair
	
	if (UOHDSettingsLocal* Settings = UOHDSettingsLocal::Get())
	{
		// Register the pair with the settings, but do not activate it yet
		if (const UPlayerMappableInputConfig* LoadedConfig = AssetManager.GetAsset(Pair.Config))
		{
			Settings->RegisterInputConfig(Pair.Type, LoadedConfig, false);
			return true;
		}	
	}
	
	return false;
}

bool FMappableConfigPair::ActivatePair(const FMappableConfigPair& Pair)
{
	UOHDAssetManager& AssetManager = UOHDAssetManager::Get();//todo make custom manager
	// Only activate a pair that has been successfully registered
	if (FMappableConfigPair::RegisterPair(Pair) && Pair.CanBeActivated())
	{		
		if (UOHDSettingsLocal* Settings = UOHDSettingsLocal::Get())//todo from GameUserSettings
		{
			if (const UPlayerMappableInputConfig* LoadedConfig = AssetManager.GetAsset(Pair.Config))
			{
				Settings->ActivateInputConfig(LoadedConfig);//todo primary activate input config in doggy component
				return true;
			}			
		}
	}
	return false;
}

void FMappableConfigPair::DeactivatePair(const FMappableConfigPair& Pair)
{
	UOHDAssetManager& AssetManager = UOHDAssetManager::Get();
	
	if (UOHDSettingsLocal* Settings = UOHDSettingsLocal::Get())
	{
		if (const UPlayerMappableInputConfig* LoadedConfig = AssetManager.GetAsset(Pair.Config))
		{
			Settings->DeactivateInputConfig(LoadedConfig);
		}
	}
}

void FMappableConfigPair::UnregisterPair(const FMappableConfigPair& Pair)
{
	UOHDAssetManager& AssetManager = UOHDAssetManager::Get();
	
	if (UOHDSettingsLocal* Settings = UOHDSettingsLocal::Get())
	{
		if (const UPlayerMappableInputConfig* LoadedConfig = AssetManager.GetAsset(Pair.Config))
		{
			Settings->UnregisterInputConfig(LoadedConfig);
		}
	}
}
