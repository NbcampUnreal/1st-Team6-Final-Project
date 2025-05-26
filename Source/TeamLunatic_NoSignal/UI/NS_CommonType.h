// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NS_CommonType.generated.h"

UENUM(BlueprintType)
enum class EUIProperty : uint8
{
    MenuUI,
    InGameUI,
};
UENUM(BlueprintType)
enum class ENewGameState : uint8
{
    
    CreateSaveMetaDataSlot ,
    OverwriteSaveMetaDataSlot

};

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
	HostNewGame,
	HostLoadGame,
    Disconnect,
    Resume,
    NewGame,
    LoadGame,
    SaveGame,
	AreYouSureMenu,
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
//------------------------------------------------------------------------------------------------------

USTRUCT(BlueprintType)
struct FSaveMetaData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    FString SaveGameSlotName;

     UPROPERTY(BlueprintReadWrite)
     FString LevelName;

    UPROPERTY(BlueprintReadWrite)
    FString SaveDate;

    UPROPERTY(BlueprintReadWrite)
    FDateTime SaveTime;
    //UPROPERTY(BlueprintReadWrite)
    //FString SaveTime;

   /*  UPROPERTY(BlueprintReadWrite)
     FDateTime SaveTime;*/
};

USTRUCT(BlueprintType)
struct FPlayerSaveData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    FString PlayerName;

    UPROPERTY(BlueprintReadWrite)
    float Health;

    UPROPERTY(BlueprintReadWrite)
    FVector SavePosition;
};

USTRUCT(BlueprintType)
struct FLevelSaveData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    FString LevelName;

    UPROPERTY(BlueprintReadWrite)
	FVector TempClearKeyItemPosition; // 예시로 추가한 키 아이템 위치
};


//class TEAMLUNATIC_NOSIGNAL_API NS_CommonType
//{
//public:
//	CommonType();
//	~CommonType();
//};
