// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MMO_Simulator : ModuleRules
{
	public MMO_Simulator(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "GameplayTags", "NavigationSystem", "AIModule", "GameplayTasks", "UMG", "Slate", "SlateCore" });

		OptimizeCode = CodeOptimization.InShippingBuildsOnly;
	}
}
