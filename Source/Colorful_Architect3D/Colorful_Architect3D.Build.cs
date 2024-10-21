// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Colorful_Architect3D : ModuleRules
{
	public Colorful_Architect3D(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
