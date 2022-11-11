// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OhHiDoggy : ModuleRules
{
	public OhHiDoggy(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay",
			"EnhancedInput", "AIModule", "GameplayTasks", "GameplayAbilities",
			"GameplayTags", "GameFeatures", "ModularGameplay",
		});
	}
}
