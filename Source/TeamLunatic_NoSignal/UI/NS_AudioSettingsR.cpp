// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_AudioSettingsR.h"
#include "Components/Slider.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

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
    GConfig->SetFloat(TEXT("/Script/GameUserSettings"), TEXT("MasterVolume"), MasterSlider->GetValue(), GGameIni);
    GConfig->SetFloat(TEXT("/Script/GameUserSettings"), TEXT("EffectVolume"), EffectSlider->GetValue(), GGameIni);
    GConfig->SetFloat(TEXT("/Script/GameUserSettings"), TEXT("AmbientVolume"), AmbientSlider->GetValue(), GGameIni);
    GConfig->SetFloat(TEXT("/Script/GameUserSettings"), TEXT("MusicVolume"), MusicSlider->GetValue(), GGameIni);
    GConfig->Flush(false, GGameIni);
}

void UNS_AudioSettingsR::LoadSoundSettings()
{
    float Volume = 1.f;
    if (MasterSlider && GConfig->GetFloat(TEXT("/Script/GameUserSettings"), TEXT("MasterVolume"), MasterVolume, GGameIni))
        MasterSlider->SetValue(MasterVolume);
    if (EffectSlider && GConfig->GetFloat(TEXT("/Script/GameUserSettings"), TEXT("EffectVolume"), EffectVolume, GGameIni))
        EffectSlider->SetValue(EffectVolume);
    if (AmbientSlider && GConfig->GetFloat(TEXT("/Script/GameUserSettings"), TEXT("AmbientVolume"), AmbientVolume, GGameIni))
        AmbientSlider->SetValue(AmbientVolume);
    if (MusicSlider && GConfig->GetFloat(TEXT("/Script/GameUserSettings"), TEXT("MusicVolume"), MusicVolume, GGameIni))
        MusicSlider->SetValue(MusicVolume);

}
void UNS_AudioSettingsR::UpdateVolumeValuesFromSliders()
{
    if (MasterSlider) MasterVolume = MasterSlider->GetValue();
    if (EffectSlider) EffectVolume = EffectSlider->GetValue();
    if (AmbientSlider) AmbientVolume = AmbientSlider->GetValue();
    if (MusicSlider) MusicVolume = MusicSlider->GetValue();
}
