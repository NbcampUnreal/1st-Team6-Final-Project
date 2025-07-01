// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_LoadGameMenuPanel.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"

void UNS_LoadGameMenuPanel::NativeConstruct()
{
	Super::NativeConstruct();
	DeleteSaveButton->OnClicked.AddDynamic(this, &UNS_LoadGameMenuPanel::OnDeleteSaveButtonClicked);
	LoadSaveButton->OnClicked.AddDynamic(this, &UNS_LoadGameMenuPanel::OnClickedSelectPanel);
}
void UNS_LoadGameMenuPanel::OnClickedSelectPanel()
{
	OnSlotClicked.Broadcast(this);
}
void UNS_LoadGameMenuPanel::SelectedCheck()
{
	Image_Select->SetVisibility(ESlateVisibility::Visible);
}
void UNS_LoadGameMenuPanel::UnSelectedCheck()
{
	Image_Select->SetVisibility(ESlateVisibility::Hidden);
}
void UNS_LoadGameMenuPanel::OnDeleteSaveButtonClicked()
{
	OnDeleteSlotClicked.Broadcast(this);
}
void UNS_LoadGameMenuPanel::SetSlotInfo(const FString& SlotName, const FString& LevelName, const FDateTime&  SaveTime)// FDateTime&
{
    if (SaveNameText)
    {
		/*
		SaveSlotName = SlotName;
		FString SlotTitle = FString::Printf(
            TEXT("%s / Map : %s"), *SlotName, *LevelName);*/
		Image_Select->SetVisibility(ESlateVisibility::Hidden);

        SaveNameText->SetText(FText::FromString(SlotName));

        FString Time = FString::Printf(TEXT(" %02d:%02d"), SaveTime.GetHour(), SaveTime.GetMinute());
        TimeText->SetText(FText::FromString(Time));

        FString Date = FString::Printf( TEXT("%04d-%02d-%02d"), SaveTime.GetYear(), SaveTime.GetMonth(), SaveTime.GetDay() );
        DateText->SetText( FText::FromString(Date));
    }
}
