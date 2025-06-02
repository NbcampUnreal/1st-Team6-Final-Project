// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_InGameMsg.h"
#include "Components/TextBlock.h"

void UNS_InGameMsg::NativeConstruct()
{
	Super::NativeConstruct();
}

void UNS_InGameMsg::ShowMessageText(const FString& Message)
{
    ShowWidgetD();

    TextMessage->SetText(FText::FromString(Message));
    PlayFadeAnimation();
}
void UNS_InGameMsg::PlayFadeAnimation()
{
    if (FadeAnim)
    {
        OnFadeFinishedDelegate.Unbind();
        OnFadeFinishedDelegate.BindDynamic(this, &UNS_InGameMsg::OnFadeAnimationFinished);

        UnbindAllFromAnimationFinished(FadeAnim);
        BindToAnimationFinished(FadeAnim, OnFadeFinishedDelegate);

        PlayAnimation(FadeAnim);
    }
}

void UNS_InGameMsg::OnFadeAnimationFinished()
{
    HideWidget();
}
