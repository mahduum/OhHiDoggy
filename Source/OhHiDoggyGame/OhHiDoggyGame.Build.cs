using UnrealBuildTool;

public class OhHiDoggyGame : ModuleRules
{
    public OhHiDoggyGame(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicIncludePaths.AddRange(
            new string[] {
                "OhHiDoggy"
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay",
                "EnhancedInput", "AIModule", "GameplayTasks", "GameplayAbilities",
                "GameplayTags", "GameFeatures", "ModularGameplay",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "OhHiDoggy",
                "CommonInput",
                "CommonUI"
            }
        );
    }
}