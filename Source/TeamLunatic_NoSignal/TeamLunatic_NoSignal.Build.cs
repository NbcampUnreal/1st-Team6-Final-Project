// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TeamLunatic_NoSignal : ModuleRules
{
    public TeamLunatic_NoSignal(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "UMG",
            "AIModule",
            "GameplayTasks",
            "NavigationSystem",
            "Niagara",
            "OnlineSubsystem",
            "OnlineSubsystemUtils",
            "Slate",
            "SlateCore",
            "ChaosSolverEngine",            // 지오메트리 컬렉션 깨짐 구현용
            "GeometryCollectionEngine"
        });

        DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");

        PublicIncludePaths.AddRange(new string[] { "TeamLunatic_NoSignal" });
    }
}
