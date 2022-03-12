// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MMO_SimulatorEditor : ModuleRules
{
	public MMO_SimulatorEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "GameplayTags", 
            "NavigationSystem", "AIModule", "GameplayTasks", "UMG", "Slate", "SlateCore", "UnrealEd", "ClassViewer", 
            "Blutility", "UMGEditor", "EditorScriptingUtilities" });

		OptimizeCode = CodeOptimization.InShippingBuildsOnly;
		
		PublicIncludePaths.AddRange(
            new string[]
            {
                "MMO_SimulatorEditor/Public"
            });

        PrivateIncludePaths.AddRange(
            new string[] 
            {
                "MMO_SimulatorEditor/Private"
            });
		
		PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "MMO_Simulator"
            });
        PrivateDependencyModuleNames.AddRange(
            new string[] 
            {
            });

        PrivateIncludePathModuleNames.AddRange(
            new string[]
            {
            });

        DynamicallyLoadedModuleNames.AddRange(
            new string[] 
            {
            });
	}
}
