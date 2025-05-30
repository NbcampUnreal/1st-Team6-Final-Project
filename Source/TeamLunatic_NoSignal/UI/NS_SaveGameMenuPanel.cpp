// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_SaveGameMenuPanel.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UNS_SaveGameMenuPanel::NativeConstruct()
{
	Super::NativeConstruct();

	DeleteSaveButton->OnClicked.AddDynamic(this, &UNS_SaveGameMenuPanel::OnClickedDeleteSaveButton);
	SaveGameButton->OnClicked.AddDynamic(this, &UNS_SaveGameMenuPanel::OnClickedSave);
}
void UNS_SaveGameMenuPanel::OnClickedDeleteSaveButton()
{

}
void UNS_SaveGameMenuPanel::OnClickedSave()
{

}
void UNS_SaveGameMenuPanel::SetSlotInfo(const FString& SlotName, const FString& LevelName, const FDateTime& SaveTime)// FDateTime&
{
    if (SaveNameText)
    {
        /*
        SaveSlotName = SlotName;
        FString SlotTitle = FString::Printf(
            TEXT("%s / Map : %s"), *SlotName, *LevelName);*/
        //Image_Select->SetVisibility(ESlateVisibility::Hidden);

        SaveNameText->SetText(FText::FromString(SlotName));

        FString Time = FString::Printf(TEXT(" %02d:%02d"), SaveTime.GetHour(), SaveTime.GetMinute());
        TimeText->SetText(FText::FromString(Time));

        FString Date = FString::Printf(TEXT("%04d-%02d-%02d"), SaveTime.GetYear(), SaveTime.GetMonth(), SaveTime.GetDay());
        DateText->SetText(FText::FromString(Date));
    }
}