#include "UI/NS_AudioSettingsR.h"
#include "Components/Slider.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"

static FString CustomIniFile;
void UNS_AudioSettingsR::NativeConstruct()
{
    Super::NativeConstruct();

#if WITH_EDITOR
    CustomIniFile = FPaths::ProjectSavedDir() / TEXT("Config/WindowsEditor/AudioSettings.ini");
#else
    CustomIniFile = FPaths::GeneratedConfigDir() / TEXT("Windows/AudioSettings.ini");
#endif

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
    FileContent += FString::Printf(TEXT("MasterVolume=%.2f\n"), MasterSlider ? MasterSlider->GetValue() : MasterVolume);
    FileContent += FString::Printf(TEXT("EffectVolume=%.2f\n"), EffectSlider ? EffectSlider->GetValue() : EffectVolume);
    FileContent += FString::Printf(TEXT("AmbientVolume=%.2f\n"), AmbientSlider ? AmbientSlider->GetValue() : AmbientVolume);
    FileContent += FString::Printf(TEXT("MusicVolume=%.2f\n"), MusicSlider ? MusicSlider->GetValue() : MusicVolume);
    if (FFileHelper::SaveStringToFile(FileContent, *CustomIniFile))
    {
        UE_LOG(LogTemp, Warning, TEXT("사운드 설정 저장 완료: %s"), *CustomIniFile);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("사운드 설정 저장 실패: %s"), *CustomIniFile);
    }
}
void UNS_AudioSettingsR::LoadSoundSettings()
{
    if (!FPaths::FileExists(CustomIniFile))
    {
        UE_LOG(LogTemp, Warning, TEXT("설정 파일이 존재하지 않습니다: %s"), *CustomIniFile);
        return;
    }
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
        if (MasterSlider)
            MasterSlider->SetValue(MasterVolume);
        if (EffectSlider)
            EffectSlider->SetValue(EffectVolume);
        if (AmbientSlider)
            AmbientSlider->SetValue(AmbientVolume);
        if (MusicSlider)
            MusicSlider->SetValue(MusicVolume);
        UE_LOG(LogTemp, Warning, TEXT("사운드 설정 로드 및 슬라이더 반영 완료"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("사운드 설정 로드 실패: %s"), *CustomIniFile);
    }
}
void UNS_AudioSettingsR::UpdateVolumeValuesFromSliders()
{
    if (MasterSlider) MasterVolume = MasterSlider->GetValue();
    if (EffectSlider) EffectVolume = EffectSlider->GetValue();
    if (AmbientSlider) AmbientVolume = AmbientSlider->GetValue();
    if (MusicSlider) MusicVolume = MusicSlider->GetValue();
}