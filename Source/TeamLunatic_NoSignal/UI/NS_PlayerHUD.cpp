// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_PlayerHUD.h"
#include "UI/NS_CircleProgressBar.h"
#include "Components/Image.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
//#include "SlateBrush.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Character/Components/NS_StatusComponent.h"
#include "UI/NS_CircleProgressBar.h"

#include "Styling/SlateColor.h"
#include "Fonts/SlateFontInfo.h"
#include "Misc/Paths.h"

void UNS_PlayerHUD::NativeConstruct()
{
    Super::NativeConstruct();

    ScrollBox_Compass->ClearChildren();

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

    TArray<FString> DirectionLabels = {
        TEXT(" N "),   TEXT(" 15 "),  TEXT(" 30 "),  TEXT(" NE "),
        TEXT(" 60 "),  TEXT(" 75 "),  TEXT(" E "),   TEXT(" 105 "),
        TEXT(" 120 "), TEXT(" SE "),  TEXT(" 150 "), TEXT(" 165 "),
        TEXT(" S "),   TEXT(" 195 "), TEXT(" 210 "), TEXT(" SW "),
        TEXT(" 240 "), TEXT(" 255 "), TEXT(" W "),   TEXT(" 285 "),
        TEXT(" 300 "), TEXT(" NW "),  TEXT(" 330 "), TEXT(" 345 "),

        TEXT(" N "),   TEXT(" 15 "),  TEXT(" 30 "),  TEXT(" NE "),
        TEXT(" 60 "),  TEXT(" 75 "),  TEXT(" E "),   TEXT(" 105 "),
        TEXT(" 120 "), TEXT(" SE "),  TEXT(" 150 "), TEXT(" 165 "),
        TEXT(" S "),   TEXT(" 195 "), TEXT(" 210 "), TEXT(" SW "),
        TEXT(" 240 "), TEXT(" 255 "), TEXT(" W "),   TEXT(" 285 "),
        TEXT(" 300 "), TEXT(" NW "),  TEXT(" 330 "), TEXT(" 345 "),

        TEXT(" N "),   TEXT(" 15 "),  TEXT(" 30 "),  TEXT(" NE "),
        TEXT(" 60 "),  TEXT(" 75 "),  TEXT(" E "),   TEXT(" 105 "),
        TEXT(" 120 "), TEXT(" SE "),  TEXT(" 150 "), TEXT(" 165 "),
        TEXT(" S "),   TEXT(" 195 "), TEXT(" 210 "), TEXT(" SW "),
        TEXT(" 240 "), TEXT(" 255 "), TEXT(" W "),   TEXT(" 285 "),
        TEXT(" 300 "), TEXT(" NW "),  TEXT(" 330 "), TEXT(" 345 "),


    };

    for (const FString& Label : DirectionLabels)
    {
        // 1. 새 TextBlock 생성
        UTextBlock* TextBlock = NewObject<UTextBlock>(this);
        if (!TextBlock) continue;

        // 2. 텍스트 설정
        TextBlock->SetText(FText::FromString(Label));

        FSlateFontInfo FontInfo = FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 18);
        FontInfo.TypefaceFontName = FName("Light");
        TextBlock->SetFont(FontInfo);

        // 정렬 설정 (가로/세로 모두 가운데)
        TextBlock->SetJustification(ETextJustify::Center);
        TextBlock->SetTextOverflowPolicy(ETextOverflowPolicy::Clip); // 텍스트 잘릴 때 정책

        // 색상 설정
        TextBlock->SetColorAndOpacity(FSlateColor(FLinearColor::White));

        // 스크롤 박스에 추가
        ScrollBox_Compass->AddChild(TextBlock);
    }


    int32 ChildCount = ScrollBox_Compass->GetChildrenCount();

    for (int32 i = 0; i < ChildCount; ++i)
    {
        UWidget* Child = ScrollBox_Compass->GetChildAt(i);
        if (UTextBlock* Text = Cast<UTextBlock>(Child))
        {
            CompassTextArray.Add(Text);
        }
    }
   // if (ScrollBox_Compass && ItemArray.IsValidIndex(49)) // 0부터 시작하니 49 = 50번째
  //      ScrollBox_Compass->ScrollWidgetIntoView(CompassTextArray[ChildCount/3], true, EDescendantScrollDestination::Center, 0.f);

    ScrollBox_Compass->SetAnimateWheelScrolling(false); // 휠 애니 끄기
    ScrollBox_Compass->SetScrollBarVisibility(ESlateVisibility::Collapsed); 

    testcheck = true;
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

void UNS_PlayerHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
   if (!testcheck)return;

    if (!GetOwningPlayer()) return;

    FRotator ControlRot = GetOwningPlayer()->GetControlRotation();
    float Yaw = FMath::Fmod(ControlRot.Yaw + 360.f, 360.f);
   
    int32 FixIndx = 24;
    if (Yaw > 180.f)
        FixIndx = 0;
    float AngleGap = 15.f;
    float CurTempIdx = Yaw / AngleGap;
    int32 CurIdx = FMath::FloorToInt(CurTempIdx);
    int32 CurFinalIdx = CurIdx + FixIndx;
    //UE_LOG(LogTemp, Warning, TEXT("CurFinalIdx = %d / Yaw = %f"), CurFinalIdx,Yaw);

    //ScrollBox_Compass->ScrollWidgetIntoView(CompassTextArray[CurFinalIdx], true, EDescendantScrollDestination::Center, 0.f);

    float MovePercent = (Yaw - (CurIdx * AngleGap)) / AngleGap;
    //float MoveDist = ((CompassTextArray[CurFinalIdx + 1]->GetDesiredSize().X / 2) + (CompassTextArray[CurFinalIdx]->GetDesiredSize().X / 2) ) * MovePercent;
    //   ScrollBox_Compass->SetScrollOffset(MoveDist);



    float Offset = 0.f;
  
    // 현재 인덱스까지의 거리 누적
    for (int32 i = 0; i < CurFinalIdx; ++i)
    {
        Offset += CompassTextArray[i]->GetDesiredSize().X;
    }

    // 현재~다음 인덱스 사이 거리 보간
    float WidthA = CompassTextArray[CurFinalIdx]->GetDesiredSize().X;
    float WidthB = CompassTextArray[CurFinalIdx + 1]->GetDesiredSize().X;

    float InterpolatedOffset = ((WidthA / 2.f) + (WidthB / 2.f)) * MovePercent;

    // View 중앙 보정
    float ViewWidth = ScrollBox_Compass->GetCachedGeometry().GetLocalSize().X;

    float FixDist = 17.f;
  /*  if (Yaw < 180.f)
        FixIndx = 0;*/

    float FinalOffset = Offset + InterpolatedOffset - (ViewWidth / 2.f)+ FixDist;

    ScrollBox_Compass->SetScrollOffset(FinalOffset);
    //UE_LOG(LogTemp, Warning, TEXT("Offset = %f / FinalOffset = %f / CurFinalIdx = %d"), Offset , FinalOffset, CurFinalIdx);

}