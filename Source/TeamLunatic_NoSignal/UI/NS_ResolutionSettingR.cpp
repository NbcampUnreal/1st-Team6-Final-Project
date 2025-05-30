// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_ResolutionSettingR.h"
#include "GameFramework/GameUserSettings.h"
#include "Engine/Engine.h"               // GEngine 사용 시 필요
#include "Misc/ConfigCacheIni.h"  

void UNS_ResolutionSettingR::NativeConstruct()
{
    //UGameUserSettings* Settings = GEngine->GetGameUserSettings();
    //if (Settings)
    //{
    //    FIntPoint Resolution = Settings->GetScreenResolution();
    //    EWindowMode::Type WindowMode = Settings->GetFullscreenMode();

    //    // 예: 드롭다운이나 슬라이더에 적용
    //    ResolutionDropdown->SetSelectedOption(GetLabelForResolution(Resolution));
    //    FullscreenCheckbox->SetIsChecked(WindowMode == EWindowMode::Fullscreen);
    //}

}
