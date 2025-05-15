// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NS_CommonType.generated.h"

UENUM(BlueprintType)
enum class EWidgetToggleType : uint8
{
    MainMenu,
    SinglePlayer,
    MultiPlayer,
    Settings,
    Video,
    Game,
    Audio,
    Controls,
    Quit,
    Back,
    ServerBrowser,
    HostServer,
    Disconnect,
    Resume,
    NewGame,
    LoadGame,
    SaveGame,
    None,
    //LoadingGame
};

UENUM(BlueprintType)
enum class EScalerSetting : uint8
{
    TextureQuality     UMETA(DisplayName = "Texture Quality"),
    AntiAliasing       UMETA(DisplayName = "Anti-Aliasing"),
    ViewDistance       UMETA(DisplayName = "ViewDistance"),
    PostProcessing     UMETA(DisplayName = "Post-Processing"),
    Effects            UMETA(DisplayName = "Effects"),
    Audio              UMETA(DisplayName = "Audio"),
    Foliage            UMETA(DisplayName = "Foliage"),
    MaxFPS             UMETA(DisplayName = "MaxFPS"),
    WindowMode         UMETA(DisplayName = "Window Mode"),
    VSync              UMETA(DisplayName = "V-Sync"),
    Shadows            UMETA(DisplayName = "Shadows")
};

USTRUCT(BlueprintType)
struct FS_Scaler
{
    GENERATED_BODY()

public:

    // 디스플레이에 보여질 텍스트 (예: "Low", "High", "Epic")
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scaler")
    FText DisplayText;

    // 해당 텍스트에 대응되는 설정 값 (예: 0.0 ~ 3.0)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scaler")
    float Value;
};





//class TEAMLUNATIC_NOSIGNAL_API NS_CommonType
//{
//public:
//	CommonType();
//	~CommonType();
//};
