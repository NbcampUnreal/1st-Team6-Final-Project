// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TeamLunatic_NoSignal : ModuleRules
{
    public TeamLunatic_NoSignal(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput","UMG", "AIModule", "GameplayTasks", "NavigationSystem", "Niagara",
    "OnlineSubsystem",
    "OnlineSubsystemUtils",
    "Sockets",
    "HTTP",
    "Json",
    "JsonUtilities",
    "Networking"});

        //if (Target.Platform == UnrealTargetPlatform.Win64)
        //{
        //    DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
        //}

        PublicIncludePaths.AddRange(new string[] { "TeamLunatic_NoSignal"});
    }
}