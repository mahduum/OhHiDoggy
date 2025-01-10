// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class OhHiDoggyEditorTarget : TargetRules
{
	public OhHiDoggyEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("OhHiDoggy");
		CppStandard = CppStandardVersion.Cpp20;
		bOverrideBuildEnvironment = true;
		bValidateFormatStrings = false;
		WindowsPlatform.bStrictConformanceMode = false;
		RegisterModulesCreatedByRider();
	}

	private void RegisterModulesCreatedByRider()
	{
	}
}
