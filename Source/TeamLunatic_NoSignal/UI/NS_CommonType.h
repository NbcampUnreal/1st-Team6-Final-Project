// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NS_CommonType.generated.h"

UENUM(BlueprintType)
enum class EWidgetToggleType : uint8
{
    SinglePlayer,
    MultiPlayer,
    Settings,
    Video,
    Game,
    Audio,
    Controls,
    Quit,
    Back,
    MainMenu,
    ServerBrowser,
    HostServer,
    Disconnect,
    Resume,
    NewGame,
    LoadGame,
    SaveGame,
    //LoadingGame
};

//class TEAMLUNATIC_NOSIGNAL_API NS_CommonType
//{
//public:
//	CommonType();
//	~CommonType();
//};
