// Fill out your copyright notice in the Description page of Project Settings.


#include "World/EndingUI/EndingResultWidget.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UEndingResultWidget::SetPlayerResultLists(const TArray<FString>& SuccessList, const TArray<FString>& FailList)
{
    if (SuccessPlayer)
    {
        SuccessPlayer->ClearChildren();
        UTextBlock* Header = NewObject<UTextBlock>(this);
        Header->SetText(FText::FromString(TEXT("탈출 성공")));
        SuccessPlayer->AddChild(Header);

        for (const FString& Name : SuccessList)
        {
            UTextBlock* NameText = NewObject<UTextBlock>(this);
            NameText->SetText(FText::FromString(Name));
            SuccessPlayer->AddChild(NameText);
        }
    }

    if (FailPlayer)
    {
        FailPlayer->ClearChildren();
        UTextBlock* Header = NewObject<UTextBlock>(this);
        Header->SetText(FText::FromString(TEXT("탈출 실패")));
        FailPlayer->AddChild(Header);

        for (const FString& Name : FailList)
        {
            UTextBlock* NameText = NewObject<UTextBlock>(this);
            NameText->SetText(FText::FromString(Name));
            FailPlayer->AddChild(NameText);
        }
    }
}

void UEndingResultWidget::SetEndingType(FName EndingType)
{
    if (RadioEndingImage)
        RadioEndingImage->SetVisibility(ESlateVisibility::Collapsed);
    if (CarEndingImage)
        CarEndingImage->SetVisibility(ESlateVisibility::Collapsed);

    if (RadionEndingText)
        RadionEndingText->SetVisibility((ESlateVisibility::Collapsed));
    if (CarEndingText)
        CarEndingText->SetVisibility((ESlateVisibility::Collapsed));
    
    if (EndingType == "Radio" && RadioEndingImage)
    {
        RadioEndingImage->SetVisibility(ESlateVisibility::Visible);
        RadionEndingText->SetVisibility((ESlateVisibility::Visible));
    }
    else if (EndingType == "Car" && CarEndingImage)
    {
        CarEndingImage->SetVisibility(ESlateVisibility::Visible);
        CarEndingText->SetVisibility((ESlateVisibility::Visible));
    }
}
