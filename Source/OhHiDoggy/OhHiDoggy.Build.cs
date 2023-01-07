// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OhHiDoggy : ModuleRules
{
	public OhHiDoggy(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] {"CommonUI"});
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "HeadMountedDisplay",
			"AIModule", "GameplayTasks", "GameplayAbilities",
			"GameplayTags", "GameFeatures", "ModularGameplay",
			"ApplicationCore"
		});
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"InputCore",
				"CoreUObject",
				"Slate",
				"SlateCore",
				"CommonInput",
				"CommonUI",
				"UMG",
				"EnhancedInput",
				"RHI",
				"AudioMixer",
				"NetCore",
				"CommonGame",
				"GameSubtitles",
				//"GameplayMessageRuntime",
			}
		);
	}
}
