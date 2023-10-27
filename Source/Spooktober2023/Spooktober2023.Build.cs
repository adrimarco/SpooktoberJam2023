// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Spooktober2023 : ModuleRules
{
	public Spooktober2023(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] 
		{ 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput",
			"AIModule",
			"GameplayTasks",
			"NavigationSystem",
			"UMG",
			"Text3D",
			"Niagara"
		});
	}
}
