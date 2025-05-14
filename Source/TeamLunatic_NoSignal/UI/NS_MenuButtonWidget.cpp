// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_MenuButtonWidget.h"
#include "NS_MenuButtonWidget.h"
#include "Components/Button.h"

void UNS_MenuButtonWidget::NativeConstruct()
{
    Super::NativeConstruct();

    //if (RootButton)
    //    RootButton->OnClicked.AddDynamic(this, &UNS_MenuButtonWidget::OnRootButtonClicked);
}

void UNS_MenuButtonWidget::OnButtonClicked()
{
    UE_LOG(LogTemp, Log, TEXT("RootButton  clicked!"));
}