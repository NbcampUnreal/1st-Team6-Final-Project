// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_PlayerHUD.h"
#include "UI/NS_CircleProgressBar.h"
#include "Components/Image.h"
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

    //TArray<UNS_CircleProgressBar*> ChildWidgets = { WBP_StatusProgressbar_Health, WBP_StatusProgressbar_Stamina, WBP_StatusProgressbar_Hunger, WBP_StatusProgressbar_Thirst, WBP_StatusProgressbar_Fatigue };

    //for (int32 i = 0; i < ChildWidgets.Num() && i < TextureArray.Num(); ++i)
    //{
    //    if (ChildWidgets[i] && TextureArray[i])
    //    {
    //        UImage* TargetImage = ChildWidgets[i]->ImageSymbol;
    //        if (TargetImage)
    //        {
    //            FSlateBrush Brush;
    //            Brush.SetResourceObject(TextureArray[i]);

    //            Brush.ImageSize = FVector2D(512, 512);

    //            TargetImage->SetBrush(Brush);
    //            TargetImage->SetBrush(Brush);
    //        }
    //    }
    //}

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
        // 1. 새 CompassElement 위젯 생성
        UNS_CompassElement* CompassElement = CreateWidget<UNS_CompassElement>(this, NS_CompassElementClass);
        if (!CompassElement || !CompassElement->TextDir) continue;

        // 2. 텍스트 설정
        CompassElement->SetDirLetter(Label);
        FSlateFontInfo FontInfo = FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 18);
        FontInfo.TypefaceFontName = FName("Light");
        CompassElement->TextDir->SetFont(FontInfo);

        // 정렬, 잘림
        CompassElement->TextDir->SetJustification(ETextJustify::Center);
        CompassElement->TextDir->SetTextOverflowPolicy(ETextOverflowPolicy::Clip);

        // 색상 설정
        CompassElement->TextDir->SetColorAndOpacity(FSlateColor(FLinearColor::White));

        // 3. 스크롤 박스에 추가
        ScrollBox_Compass->AddChild(CompassElement);
    }

    int32 ChildCount = ScrollBox_Compass->GetChildrenCount();
    for (int32 i = 0; i < ChildCount; ++i)
    {
        UWidget* Child = ScrollBox_Compass->GetChildAt(i);
        if (UNS_CompassElement* Text = Cast<UNS_CompassElement>(Child))//UNS_CompassElement UTextBlock
            CompassTextArray.Add(Text);
        else
        {
			UE_LOG(LogTemp, Warning, TEXT(" UNS_CompassElement nullptr idx : %d"),i);
        }
    }
   // if (ScrollBox_Compass && ItemArray.IsValidIndex(49)) // 0부터 시작하니 49 = 50번째
  //      ScrollBox_Compass->ScrollWidgetIntoView(CompassTextArray[ChildCount/3], true, EDescendantScrollDestination::Center, 0.f);

    ScrollBox_Compass->SetAnimateWheelScrolling(false); // 휠 애니 끄기
    ScrollBox_Compass->SetScrollBarVisibility(ESlateVisibility::Collapsed); 

    testcheck = true;
   // UE_LOG(LogTemp, Warning, TEXT("UNS_PlayerHUD ::NativeConstruct"));
}

void UNS_PlayerHUD::ShowWidget()
{
    SetVisibility(ESlateVisibility::Visible);

    APlayerController* PC = GetOwningPlayer();
    if (!PC) return;

    APawn* MyPawn = PC->GetPawn();
    if (!MyPawn)
    {
        // Pawn이 아직 없으면 다음 틱에 다시 시도
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

	YeddaItemArray.Add(YeddaItem);
}

void UNS_PlayerHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    if (!testcheck) return;
    if (!GetOwningPlayer()) return;

    FRotator ControlRot = GetOwningPlayer()->GetControlRotation();
    float Yaw = FMath::Fmod(ControlRot.Yaw + 360.f, 360.f);

    int32 FixIndx = (Yaw > 180.f) ? 0 : 24;
    float AngleGap = 15.f;
    int32 CurIdx = FMath::FloorToInt(Yaw / AngleGap);
    int32 CurFinalIdx = CurIdx + FixIndx;

    float MovePercent = (Yaw - (CurIdx * AngleGap)) / AngleGap;

    //인덱스 유효성
    bool bValidA = CompassTextArray.IsValidIndex(CurFinalIdx);
    bool bValidB = CompassTextArray.IsValidIndex(CurFinalIdx + 1);
    if (!bValidA || !bValidB)
    {
        UE_LOG(LogTemp, Warning, TEXT("Compass index out of: CurFinalIdx=%d / Num=%d"), CurFinalIdx, CompassTextArray.Num());
        return;
    }
 //   UE_LOG(LogTemp, Warning, TEXT("UNS_PlayerHUD ::NativeTick1"));
 //   UE_LOG(LogTemp, Warning, TEXT("CurFinalIdx: %d / CompassTextArray Les :  %d" ), CurFinalIdx, CompassTextArray.Num());
    // 현재 인덱스까지의 거리 누적
    float Offset = 0.f;
    for (int32 i = 0; i < CurFinalIdx; ++i)
    {
        if (CompassTextArray.IsValidIndex(i))
            Offset += CompassTextArray[i]->GetSizeBoxWideth();
    }

    float InterpolatedOffset = 0.f;
	float WidthA = 0.f;
	float WidthB = 0.f;
    if (!IsValid(CompassTextArray[CurFinalIdx]) || !IsValid(CompassTextArray[CurFinalIdx + 1]))
    {
        UE_LOG(LogTemp, Warning, TEXT("CompassTextArray !!!!!IsValid"));
        return;
    }
    else
    {
        WidthA = CompassTextArray[CurFinalIdx]->GetSizeBoxWideth();
        WidthB = CompassTextArray[CurFinalIdx + 1]->GetSizeBoxWideth();
        InterpolatedOffset = ((WidthA / 2.f) + (WidthB / 2.f)) * MovePercent;
    }

    float ViewWidth = ScrollBox_Compass->GetCachedGeometry().GetLocalSize().X;
    float FixDist = 16.f;
    float FinalOffset = Offset + InterpolatedOffset - (ViewWidth / 2.f) + FixDist;
    ScrollBox_Compass->SetScrollOffset(FinalOffset);

    //if (0 == YeddaItemArray.Num())
    //    return;

    if (!CachedPlayerCharacter || !ScrollBox_Compass) return;

	//for (APickup* YeddaItem : YeddaItemArray)
	//{
	//	if (!YeddaItem) continue;
	//	// 1. 방향 계산
	//	FVector PlayerLoc = CachedPlayerCharacter->GetActorLocation();
 //       //이건 절대 각도 계산
	//	//FVector Dir = (YeddaItem->GetActorLocation() - PlayerLoc).GetSafeNormal2D();
	//	//float Angle = FMath::RadiansToDegrees(FMath::Atan2(Dir.Y, Dir.X));
	//	//if (Angle < 0.f) Angle += 360.f;
 //   
 //       //--------------------------------------------------------
 //       FVector Forward = CachedPlayerCharacter->GetActorForwardVector().GetSafeNormal2D();
 //       FVector ToItem = (YeddaItem->GetActorLocation() - PlayerLoc).GetSafeNormal2D();

 //       //float AngleRad = FMath::Acos(FVector::DotProduct(Forward, ToItem));
 //       //float AngleDeg = FMath::RadiansToDegrees(AngleRad);

 //       //// 방향 판별 (왼쪽/오른쪽)
 //       //FVector Cross = FVector::CrossProduct(Forward, ToItem);
 //       //if (Cross.Z < 0)
 //       //    AngleDeg = -AngleDeg;
 //       //
 //       
 //       // 최종 상대각: 내 정면이 0°, 왼쪽은 음수, 오른쪽은 양수
 //       //float RelativeAngle = FMath::Fmod(AngleDeg + 360.f, 360.f);
 //       //----------------------------------------------------------------------
 //       float ItemAngle = FMath::RadiansToDegrees(FMath::Atan2(ToItem.Y, ToItem.X));
 //       if (ItemAngle < 0.f) ItemAngle += 360.f;

 //       // 현재 카메라 기준 상대각 (0~360도)
 //       float RelativeAngle = FMath::Fmod(ItemAngle - Yaw + 360.f, 360.f);

 //       float SegmentAngle = 15.f;
 //       int32 Index = ( FMath::FloorToInt((RelativeAngle + SegmentAngle * 0.5f) / SegmentAngle) % 24 ) ;

	//	// 3. 인덱스가 같으면 아무것도 안 함 (성능 최적화)
	///*	if (Index == LastHighlightedIndex) return;
	//	LastHighlightedIndex = Index;*/
	//	// 4. UI 갱신
	//	int32 Count = ScrollBox_Compass->GetChildrenCount();
	//	for (int32 i = 0; i < Count; ++i)
	//	{
	//		if (UNS_CompassElement* Elem = Cast<UNS_CompassElement>(ScrollBox_Compass->GetChildAt(i)))
	//		{
	//			bool bHighlight = (i == Index) || (i == Index+24);
	//			Elem->TextDir->SetColorAndOpacity(FSlateColor(bHighlight ? FLinearColor::Red : FLinearColor::White));

 //               if (CurFinalIdx != i && PrvFinalIdx != i)
 //                  bHighlight ? Elem->SetTextScale(1.2f) : Elem->ReturnTextScale();
	//		}
	//	}

    TArray<int32> HighlightIndices;
    for (APickup* YeddaItem : YeddaItemArray)
    {
        if (!YeddaItem) continue;

        FVector PlayerLoc = CachedPlayerCharacter->GetActorLocation();
        FVector ToItem = (YeddaItem->GetActorLocation() - PlayerLoc).GetSafeNormal2D();

        float ItemAngle = FMath::RadiansToDegrees(FMath::Atan2(ToItem.Y, ToItem.X));
        if (ItemAngle < 0.f) ItemAngle += 360.f;

        float RelativeAngle = FMath::Fmod(ItemAngle - Yaw + 360.f, 360.f);

        float SegmentAngle = 15.f;
        int32 Index = (FMath::FloorToInt((RelativeAngle + SegmentAngle * 0.5f) / SegmentAngle) % 24);

        HighlightIndices.Add(Index);
        HighlightIndices.Add(Index + 24);
    }

    int32 Count = ScrollBox_Compass->GetChildrenCount();
    for (int32 i = 0; i < Count; ++i)
    {
        if (UNS_CompassElement* Elem = Cast<UNS_CompassElement>(ScrollBox_Compass->GetChildAt(i)))
        {
            bool bHighlight = HighlightIndices.Contains(i);

            Elem->TextDir->SetColorAndOpacity(FSlateColor(bHighlight ? FLinearColor::Red : FLinearColor::White));

            if (Elem->Image_Arrow)
            {
                Elem->Image_Arrow->SetVisibility(bHighlight ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
            }

            if (CurFinalIdx != i && PrvFinalIdx != i)
            {
                bHighlight ? Elem->SetTextScale(1.2f) : Elem->ReturnTextScale();
            }
        }
    }

    if (0.5f < MovePercent)
        CompassTextArray[CurFinalIdx + 1]->SetTextScale(1.22f);
    else if (0.5f > MovePercent)
        CompassTextArray[CurFinalIdx]->SetTextScale(1.22f);
    else
    {
        CompassTextArray[CurFinalIdx]->ReturnTextScale();
		CompassTextArray[CurFinalIdx + 1]->ReturnTextScale();
    }
      
        PrvFinalIdx = CurFinalIdx;
        // UE_LOG(LogTemp, Warning, TEXT("UNS_PlayerHUD ::NativeTick3"));
}


void UNS_PlayerHUD::DeleteCompasItem(APickup* DeleteItem)
{
	if (!DeleteItem || !CachedPlayerCharacter) return;
	if (YeddaItemArray.Contains(DeleteItem))
	{
		YeddaItemArray.Remove(DeleteItem);
		//UE_LOG(LogTemp, Warning, TEXT(" DeleteItem: %s"), *DeleteItem->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT(" DeleteItem not found: %s"), *DeleteItem->GetName());
	}
}

