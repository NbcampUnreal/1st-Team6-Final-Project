// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_PlayerHUD.h"
#include "UI/NS_CircleProgressBar.h"
#include "Components/Image.h"
//#include "SlateBrush.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Character/Components/NS_StatusComponent.h"
#include "UI/NS_CircleProgressBar.h"

void UNS_PlayerHUD::NativeConstruct()
{
    Super::NativeConstruct();

    TArray<UNS_CircleProgressBar*> ChildWidgets = { WBP_StatusProgressbar_Health, WBP_StatusProgressbar_Stamina, WBP_StatusProgressbar_Hunger, WBP_StatusProgressbar_Thirst, WBP_StatusProgressbar_Fatigue };

    for (int32 i = 0; i < ChildWidgets.Num() && i < TextureArray.Num(); ++i)
    {
        if (ChildWidgets[i] && TextureArray[i])
        {
            UImage* TargetImage = ChildWidgets[i]->ImageSymbol;
            if (TargetImage)
            {
                FSlateBrush Brush;
                Brush.SetResourceObject(TextureArray[i]);

                Brush.ImageSize = FVector2D(512, 512);

                TargetImage->SetBrush(Brush);
                TargetImage->SetBrush(Brush);
            }
        }
    }

}

void UNS_PlayerHUD::ShowWidget()
{
    SetVisibility(ESlateVisibility::Visible);

    if (APlayerController* PC = GetOwningPlayer())
    {
        if (APawn* MyPawn = PC->GetPawn())
        {
            CachedMyCharacter = Cast<ANS_PlayerCharacterBase>(MyPawn);
        }
    }

    GetWorld()->GetTimerManager().ClearTimer(UpdatePlayerStausHandle);
    TWeakObjectPtr<UNS_PlayerHUD> SafeThis = this;
    float ElapsedTime = 0.f;
    float PrevTime = GetWorld()->GetTimeSeconds();
    GetWorld()->GetTimerManager().SetTimer
    (
        UpdatePlayerStausHandle,
        FTimerDelegate::CreateLambda([SafeThis, ElapsedTime, PrevTime]() mutable
        {
            if (!SafeThis.IsValid())return;

            UWorld* World = SafeThis->GetWorld();

            if (!World) return;
           
            SafeThis->WBP_StatusProgressbar_Health->UpdatePercent( SafeThis->CachedMyCharacter->StatusComp->Health*0.01f );
            SafeThis->WBP_StatusProgressbar_Stamina->UpdatePercent( SafeThis->CachedMyCharacter->StatusComp->Stamina * 0.01f);
        }), 0.05f, true
    );
}

void UNS_PlayerHUD::HideWidget()
{
    SetVisibility(ESlateVisibility::Hidden);
}
