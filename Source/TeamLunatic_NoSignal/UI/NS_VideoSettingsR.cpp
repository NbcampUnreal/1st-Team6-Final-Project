// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_VideoSettingsR.h"
#include "GameFramework/GameUserSettings.h"
#include "Engine/Engine.h"               // GEngine 사용 시 필요
#include "Misc/ConfigCacheIni.h"         // (선택적, .ini 관련 작업 시)
#include "UI/NS_ResolutionSettingR.h"
#include "Components/TextBlock.h"

void UNS_VideoSettingsR::NativeConstruct()
{
}

void UNS_VideoSettingsR::ShowWidget()
{
	Super::ShowWidget();

    LoadVideoSetting();

    UGameUserSettings* Settings = GEngine->GetGameUserSettings();
    if (Settings)
    {
        FIntPoint Resolution = Settings->GetScreenResolution();
        FString ResolutionText = FString::Printf(TEXT("%dx%d"), Resolution.X, Resolution.Y);
        BP_ResolutionSelector->ScalerText->SetText(FText::FromString(ResolutionText));

        EWindowMode::Type WindowMode = Settings->GetFullscreenMode();

     
    }
}

