// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_AudioSettingsR.h"
#include "Components/Slider.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

static  FString CustomIniFile = FPaths::ProjectSavedDir() / TEXT("Config/WindowsEditor/GameUserSettings1.ini");

void UNS_AudioSettingsR::NativeConstruct()
{
	Super::NativeConstruct();

    LoadSoundSettings();
    ApplyVolumes();

    if (SaveButton)
        SaveButton->OnClicked.AddDynamic(this, &UNS_AudioSettingsR::OnSaveButtonClicked);

    if (ResetButton)
        ResetButton->OnClicked.AddDynamic(this, &UNS_AudioSettingsR::OnResetButtonClicked);

    if (MasterSlider)
        MasterSlider->OnValueChanged.AddDynamic(this, &UNS_AudioSettingsR::OnSliderValueChanged);
    if (EffectSlider)
        EffectSlider->OnValueChanged.AddDynamic(this, &UNS_AudioSettingsR::OnSliderValueChanged);
    if (AmbientSlider)
        AmbientSlider->OnValueChanged.AddDynamic(this, &UNS_AudioSettingsR::OnSliderValueChanged);
    if (MusicSlider)
        MusicSlider->OnValueChanged.AddDynamic(this, &UNS_AudioSettingsR::OnSliderValueChanged);

}
void UNS_AudioSettingsR::OnSliderValueChanged(float)
{
    ApplyVolumes();
}

void UNS_AudioSettingsR::OnResetButtonClicked()
{
    MasterVolume = 0.5f;
    EffectVolume = 0.5f;
    AmbientVolume = 0.5f;
    MusicVolume = 0.5f;

    if (MasterSlider)
	    MasterSlider->SetValue(MasterVolume);
	if (EffectSlider)
		EffectSlider->SetValue(EffectVolume);
	if (AmbientSlider)
		AmbientSlider->SetValue(AmbientVolume);
	if (MusicSlider)
		MusicSlider->SetValue(MusicVolume);
}

void UNS_AudioSettingsR::ApplyVolumes()
{
    if (MasterSlider)
        UGameplayStatics::SetSoundMixClassOverride(this, nullptr, nullptr, MasterSlider->GetValue());
    if (EffectSlider)
        UGameplayStatics::SetSoundMixClassOverride(this, nullptr, nullptr, EffectSlider->GetValue());
    if (AmbientSlider)
        UGameplayStatics::SetSoundMixClassOverride(this, nullptr, nullptr, AmbientSlider->GetValue());
    if (MusicSlider)
        UGameplayStatics::SetSoundMixClassOverride(this, nullptr, nullptr, MusicSlider->GetValue());
}

void UNS_AudioSettingsR::OnSaveButtonClicked()
{
    SaveSoundSettings();
}

void UNS_AudioSettingsR::SaveSoundSettings()
{
    FString FileContent;
    FileContent += TEXT("[Audio]\n");
    FileContent += FString::Printf(TEXT("MasterVolume=%.2f\n"), MasterSlider->GetValue());
    FileContent += FString::Printf(TEXT("EffectVolume=%.2f\n"), EffectSlider->GetValue());
    FileContent += FString::Printf(TEXT("AmbientVolume=%.2f\n"), AmbientSlider->GetValue());
    FileContent += FString::Printf(TEXT("MusicVolume=%.2f\n"), MusicSlider->GetValue());

    FFileHelper::SaveStringToFile(FileContent, *CustomIniFile);

    UE_LOG(LogTemp, Warning, TEXT("직접 파일 생성 및 저장 완료: %s"), *CustomIniFile);

    //GConfig->SetFloat(TEXT("Audio"), TEXT("MasterVolume"), MasterSlider->GetValue(), CustomIniFile);
    //GConfig->SetFloat(TEXT("Audio"), TEXT("EffectVolume"), EffectSlider->GetValue(), CustomIniFile);
    //GConfig->SetFloat(TEXT("Audio"), TEXT("AmbientVolume"), AmbientSlider->GetValue(), CustomIniFile);
    //GConfig->SetFloat(TEXT("Audio"), TEXT("MusicVolume"), MusicSlider->GetValue(), CustomIniFile);
    //GConfig->Flush(false, CustomIniFile);
    //UE_LOG(LogTemp, Warning, TEXT("저장 파일 경로: %s"), *CustomIniFile);
}

void UNS_AudioSettingsR::LoadSoundSettings()
{
    FString FileContent;
    if (FFileHelper::LoadFileToString(FileContent, *CustomIniFile))
    {
        TArray<FString> Lines;
        FileContent.ParseIntoArrayLines(Lines);
        for (const FString& Line : Lines)
        {
            float Value = 0.f;
            if (Line.StartsWith(TEXT("MasterVolume=")))
                MasterVolume = FCString::Atof(*Line.RightChop(13));
            else if (Line.StartsWith(TEXT("EffectVolume=")))
                EffectVolume = FCString::Atof(*Line.RightChop(13));
            else if (Line.StartsWith(TEXT("AmbientVolume=")))
                AmbientVolume = FCString::Atof(*Line.RightChop(14));
            else if (Line.StartsWith(TEXT("MusicVolume=")))
                MusicVolume = FCString::Atof(*Line.RightChop(12));
        }
        // 슬라이더 반영
        MasterSlider->SetValue(MasterVolume);
        EffectSlider->SetValue(EffectVolume);
        AmbientSlider->SetValue(AmbientVolume);
        MusicSlider->SetValue(MusicVolume);
        UE_LOG(LogTemp, Warning, TEXT("직접 파일 로드 및 슬라이더 반영 완료"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("파일 로드 실패: %s"), *CustomIniFile);
    }

    //float Volume = 1.f;
    //if (MasterSlider && GConfig->GetFloat(TEXT("Audio"), TEXT("MasterVolume"), MasterVolume, CustomIniFile))
    //    MasterSlider->SetValue(MasterVolume);
    //if (EffectSlider && GConfig->GetFloat(TEXT("Audio"), TEXT("EffectVolume"), EffectVolume, CustomIniFile))
    //    EffectSlider->SetValue(EffectVolume);
    //if (AmbientSlider && GConfig->GetFloat(TEXT("Audio"), TEXT("AmbientVolume"), AmbientVolume, CustomIniFile))
    //    AmbientSlider->SetValue(AmbientVolume);
    //if (MusicSlider && GConfig->GetFloat(TEXT("Audio"), TEXT("MusicVolume"), MusicVolume, CustomIniFile))
    //    MusicSlider->SetValue(MusicVolume);
}
void UNS_AudioSettingsR::UpdateVolumeValuesFromSliders()
{
    if (MasterSlider) MasterVolume = MasterSlider->GetValue();
    if (EffectSlider) EffectVolume = EffectSlider->GetValue();
    if (AmbientSlider) AmbientVolume = AmbientSlider->GetValue();
    if (MusicSlider) MusicVolume = MusicSlider->GetValue();
}
