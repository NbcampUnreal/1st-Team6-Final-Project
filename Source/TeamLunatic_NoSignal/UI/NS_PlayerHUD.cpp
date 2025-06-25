// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_PlayerHUD.h"
#include "UI/NS_CircleProgressBar.h"
#include "Components/Image.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"
//#include "SlateBrush.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Character/Components/NS_StatusComponent.h"
#include "UI/NS_CircleProgressBar.h"
#include "UI/NS_CompassElement.h"
#include "Styling/SlateColor.h"
#include "Fonts/SlateFontInfo.h"
#include "Misc/Paths.h"
#include "World/Pickup.h"
#include "Character/NS_PlayerController.h"

const float BaseWidth = 35; // 기존 WidthOverride 값
const float EnlargedWidth = BaseWidth * 1.2f;

void UNS_PlayerHUD::NativeConstruct()
{
    Super::NativeConstruct();

    ScrollBox_Compass->ClearChildren();
    CompassTextArray.Empty();

    TArray<FString> DirectionLabels = {
        TEXT(" N "),  TEXT(" 15 "), TEXT(" 30 "), TEXT(" NE "),
        TEXT(" 60 "), TEXT(" 75 "), TEXT(" E "),  TEXT(" 105 "),
        TEXT(" 120 "),TEXT(" SE "), TEXT(" 150 "),TEXT(" 165 "),
        TEXT(" S "),  TEXT(" 195 "),TEXT(" 210 "),TEXT(" SW "),
        TEXT(" 240 "),TEXT(" 255 "),TEXT(" W "),  TEXT(" 285 "),
        TEXT(" 300 "),TEXT(" NW "), TEXT(" 330 "),TEXT(" 345 "),

        TEXT(" N "),  TEXT(" 15 "), TEXT(" 30 "), TEXT(" NE "),
        TEXT(" 60 "), TEXT(" 75 "), TEXT(" E "),  TEXT(" 105 "),
        TEXT(" 120 "),TEXT(" SE "), TEXT(" 150 "),TEXT(" 165 "),
        TEXT(" S "),  TEXT(" 195 "),TEXT(" 210 "),TEXT(" SW "),
        TEXT(" 240 "),TEXT(" 255 "),TEXT(" W "),  TEXT(" 285 "),
        TEXT(" 300 "),TEXT(" NW "), TEXT(" 330 "),TEXT(" 345 "),

        TEXT(" N "),  TEXT(" 15 "), TEXT(" 30 "), TEXT(" NE "),
        TEXT(" 60 "), TEXT(" 75 "), TEXT(" E "),  TEXT(" 105 "),
        TEXT(" 120 "),TEXT(" SE "), TEXT(" 150 "),TEXT(" 165 "),
        TEXT(" S "),  TEXT(" 195 "),TEXT(" 210 "),TEXT(" SW "),
        TEXT(" 240 "),TEXT(" 255 "),TEXT(" W "),  TEXT(" 285 "),
        TEXT(" 300 "),TEXT(" NW "), TEXT(" 330 "),TEXT(" 345 "),
    };

    for (const FString& Label : DirectionLabels)
    {
        UNS_CompassElement* CompassElement = CreateWidget<UNS_CompassElement>(this, NS_CompassElementClass);
        if (!CompassElement || !CompassElement->TextDir) continue;

        CompassElement->SetDirLetter(Label);
        FSlateFontInfo FontInfo = FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 18);
        FontInfo.TypefaceFontName = FName("Light");
        CompassElement->TextDir->SetFont(FontInfo);
        CompassElement->TextDir->SetJustification(ETextJustify::Center);
        CompassElement->TextDir->SetTextOverflowPolicy(ETextOverflowPolicy::Clip);
        CompassElement->TextDir->SetColorAndOpacity(FSlateColor(FLinearColor::White));

        ScrollBox_Compass->AddChild(CompassElement);
    }

    for (int32 i = 0; i < ScrollBox_Compass->GetChildrenCount(); ++i)
    {
        if (UNS_CompassElement* Text = Cast<UNS_CompassElement>(ScrollBox_Compass->GetChildAt(i)))
        {
            CompassTextArray.Add(Text);
        }
    }

    ScrollBox_Compass->SetAnimateWheelScrolling(false);
    ScrollBox_Compass->SetScrollBarVisibility(ESlateVisibility::Collapsed);

    testcheck = true;
}
void UNS_PlayerHUD::ShowWidget()
{
    SetVisibility(ESlateVisibility::Visible);

    APlayerController* PC = GetOwningPlayer();
    if (!PC) return;

    APawn* MyPawn = PC->GetPawn();
    if (!MyPawn)
    {
        FTimerHandle RetryHandle;
        GetWorld()->GetTimerManager().SetTimer(
            RetryHandle,
            FTimerDelegate::CreateLambda([this]() {
            this->ShowWidget();
        }),
            0.1f,
            false
        );
        return;
    }

    CachedPlayerCharacter = Cast<ANS_PlayerCharacterBase>(MyPawn);
    if (!CachedPlayerCharacter) return;

    GetWorld()->GetTimerManager().ClearTimer(UpdatePlayerStausHandle);
    TWeakObjectPtr<UNS_PlayerHUD> SafeThis = this;
    float ElapsedTime = 0.f;
    float PrevTime = GetWorld()->GetTimeSeconds();

    GetWorld()->GetTimerManager().SetTimer
    (
        UpdatePlayerStausHandle,
        FTimerDelegate::CreateLambda([SafeThis]() mutable
    {
        if (!SafeThis.IsValid()) return;
        if (!SafeThis->GetWorld()) return;
        if (!SafeThis->CachedPlayerCharacter || !SafeThis->CachedPlayerCharacter->StatusComp) return;

       // SafeThis->WBP_StatusProgressbar_Health->UpdatePercent(SafeThis->CachedPlayerCharacter->StatusComp->Health * 0.01f);
       // SafeThis->WBP_StatusProgressbar_Stamina->UpdatePercent(SafeThis->CachedPlayerCharacter->StatusComp->Stamina * 0.01f);
        SafeThis->ProgressBar_Stamina->SetPercent(SafeThis->CachedPlayerCharacter->StatusComp->Stamina * 0.01f);
        SafeThis->ProgressBar_Health->SetPercent(SafeThis->CachedPlayerCharacter->StatusComp->Health * 0.01f);
    }),
        0.05f,
        true
    );
}

void UNS_PlayerHUD::HideWidget()
{
    SetVisibility(ESlateVisibility::Hidden);
}

void UNS_PlayerHUD::SetYeddaItem(APickup* YeddaItem)
{
    if (!CachedPlayerCharacter || !YeddaItem || !ScrollBox_Compass) return;

    if (!YeddaItemArray.Contains(YeddaItem))
    {
        YeddaItemArray.Add(YeddaItem);
    }
}

void UNS_PlayerHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    // Geometry 안전성 검사 (크래시 방지)
    const FGeometry& CompassGeo = ScrollBox_Compass->GetCachedGeometry();
    if (!CompassGeo.IsUnderLocation(FVector2D::ZeroVector) || CompassGeo.GetLocalSize().X <= 0.f)
    {
        return;
    }
    
    if (!testcheck || !GetOwningPlayer() || !CachedPlayerCharacter || !ScrollBox_Compass || CompassTextArray.Num() < 72)
    {
        return;
    }

    const float PlayerYaw = FRotator::NormalizeAxis(GetOwningPlayer()->GetControlRotation().Yaw);
    const float AngleGap = 15.f;

    const float YawForScrolling = FMath::Fmod(PlayerYaw + 360.f, 360.f);
    const int32 CurrentIndex = FMath::FloorToInt(YawForScrolling / AngleGap);
    const float InterpFraction = FMath::Fmod(YawForScrolling / AngleGap, 1.f);
    const int32 FinalIndex = CurrentIndex + 24;

    if (!CompassTextArray.IsValidIndex(FinalIndex) || !CompassTextArray.IsValidIndex(FinalIndex + 1))
    {
        return;
    }

    if (!IsValid(CompassTextArray[FinalIndex]) || !IsValid(CompassTextArray[FinalIndex + 1]))
    {
        return;
    }

    float BaseOffset = 0.f;
    for (int32 i = 0; i < FinalIndex; ++i)
    {
    
        if (IsValid(CompassTextArray[i]))
        {
            BaseOffset += CompassTextArray[i]->GetCachedGeometry().GetLocalSize().X;
        }
    }

    const float WidthA = CompassTextArray[FinalIndex]->GetCachedGeometry().GetLocalSize().X;
    const float WidthB = CompassTextArray[FinalIndex + 1]->GetCachedGeometry().GetLocalSize().X;
    const float InterpolatedOffset = ((WidthA / 2.f) + (WidthB / 2.f)) * InterpFraction;

    const float ViewWidth = ScrollBox_Compass->GetCachedGeometry().GetLocalSize().X;
    float TargetOffset = BaseOffset + InterpolatedOffset - (ViewWidth / 2.f) + (WidthA / 2.f);

    ScrollBox_Compass->SetScrollOffset(TargetOffset);

    TArray<int32> HighlightIndices;
    const FVector PlayerLoc = CachedPlayerCharacter->GetActorLocation();

    for (APickup* YeddaItem : YeddaItemArray)
    {
        if (!IsValid(YeddaItem)) continue; 

        const FVector ToItem = (YeddaItem->GetActorLocation() - PlayerLoc).GetSafeNormal2D();
        const float ItemWorldYaw = FMath::RadiansToDegrees(FMath::Atan2(ToItem.Y, ToItem.X));

        int32 Index = FMath::RoundToInt(FMath::Fmod(ItemWorldYaw + 360.f, 360.f) / AngleGap) % 24;

        HighlightIndices.Add(Index);
        HighlightIndices.Add(Index + 24);
        HighlightIndices.Add(Index + 48);
    }

    for (int32 i = 0; i < CompassTextArray.Num(); ++i)
    {
        if (UNS_CompassElement* Elem = CompassTextArray.IsValidIndex(i) ? CompassTextArray[i] : nullptr; IsValid(Elem))
        {
            bool bHighlight = HighlightIndices.Contains(i);

            // TextDir 포인터가 유효한지 확인
            if (IsValid(Elem->TextDir))
            {
                Elem->TextDir->SetColorAndOpacity(FSlateColor(bHighlight ? FLinearColor::Red : FLinearColor::White));
            }

            // Image_Arrow 포인터가 유효한지 확인
            if (IsValid(Elem->Image_Arrow))
            {
                Elem->Image_Arrow->SetVisibility(bHighlight ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
            }
        }
    }

}

void UNS_PlayerHUD::DeleteCompasItem(APickup* DeleteItem)
{
    if (YeddaItemArray.Contains(DeleteItem))
    {
        YeddaItemArray.Remove(DeleteItem);
    }
}

void UNS_PlayerHUD::SetTipText(const FText& NewText)
{
    if (IsValid(TipText))
    {
        TipText->SetText(NewText);
    }
}