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
    "Networking",
    "Slate",
    "SlateCore",
    "ChaosSolverEngine", // 지오 메트리 컬렉션 사용하려고 던지는 액터는 깨지는거 구현 용
    "GeometryCollectionEngine" // 지오 메트리 컬렉션 사용하려고 던지는 액터는 깨지는거 구현 용
            });

        //if (Target.Platform == UnrealTargetPlatform.Win64)
        //{
        //    DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
        //}

        PublicIncludePaths.AddRange(new string[] { "TeamLunatic_NoSignal"});
    }
}