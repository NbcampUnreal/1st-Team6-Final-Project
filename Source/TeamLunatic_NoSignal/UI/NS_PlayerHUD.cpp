// NS_PlayerHUD.cpp - 플레이어의 게임 내 HUD(Head-Up Display) 위젯을 관리하는 클래스

#include "UI/NS_PlayerHUD.h"
#include "UI/NS_CircleProgressBar.h"
#include "Components/Image.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "GameFlow/NS_GameInstance.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"
#include "UI/NS_UIManager.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Character/Components/NS_StatusComponent.h"
#include "UI/NS_CircleProgressBar.h"
#include "UI/NS_CompassElement.h"
#include "Styling/SlateColor.h"
#include "Fonts/SlateFontInfo.h"
#include "Misc/Paths.h"
#include "World/Pickup.h"
#include "Character/NS_PlayerController.h"

// 나침반 요소의 기본 너비 값
const float BaseWidth = 35;
// 확대된 너비 값 (기본 너비의 1.2배)
const float EnlargedWidth = BaseWidth * 1.2f;

void UNS_PlayerHUD::NativeConstruct()
{
    // 부모 클래스의 NativeConstruct 호출
    Super::NativeConstruct();

    // UI 매니저에 현재 HUD 위젯 등록
    if (UNS_GameInstance* GI = GetGameInstance<UNS_GameInstance>())
    {
        if (UNS_UIManager* UIManager = GI->GetUIManager())
        {
            UIManager->SetPlayerHUDWidget(this);
        }
    }

    // 나침반 초기화: 기존 요소 제거 및 배열 초기화
    ScrollBox_Compass->ClearChildren();
    CompassTextArray.Empty();

    // 나침반 방향 라벨 정의 (N, NE, E, SE, S, SW, W, NW 및 각도)
    TArray<FString> DirectionLabels = {
        TEXT(" N "),  TEXT(" 15 "), TEXT(" 30 "), TEXT(" NE "),
        TEXT(" 60 "), TEXT(" 75 "), TEXT(" E "),  TEXT(" 105 "),
        TEXT(" 120 "),TEXT(" SE "), TEXT(" 150 "),TEXT(" 165 "),
        TEXT(" S "),  TEXT(" 195 "),TEXT(" 210 "),TEXT(" SW "),
        TEXT(" 240 "),TEXT(" 255 "),TEXT(" W "),  TEXT(" 285 "),
        TEXT(" 300 "),TEXT(" NW "), TEXT(" 330 "),TEXT(" 345 "),

        // 나침반 스크롤을 위해 방향 라벨 반복 (3번 반복)
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

    // 각 방향 라벨에 대한 나침반 요소 생성 및 설정
    for (const FString& Label : DirectionLabels)
    {
        UNS_CompassElement* CompassElement = CreateWidget<UNS_CompassElement>(this, NS_CompassElementClass);
        if (!CompassElement || !CompassElement->TextDir) continue;

        // 방향 텍스트 설정
        CompassElement->SetDirLetter(Label);
        
        // 폰트 스타일 설정
        FSlateFontInfo FontInfo = FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 18);
        FontInfo.TypefaceFontName = FName("Light");
        CompassElement->TextDir->SetFont(FontInfo);
        CompassElement->TextDir->SetJustification(ETextJustify::Center);
        CompassElement->TextDir->SetTextOverflowPolicy(ETextOverflowPolicy::Clip);
        CompassElement->TextDir->SetColorAndOpacity(FSlateColor(FLinearColor::White));

        // 스크롤 박스에 요소 추가
        ScrollBox_Compass->AddChild(CompassElement);
    }

    // 생성된 나침반 요소들을 배열에 저장
    for (int32 i = 0; i < ScrollBox_Compass->GetChildrenCount(); ++i)
    {
        if (UNS_CompassElement* Text = Cast<UNS_CompassElement>(ScrollBox_Compass->GetChildAt(i)))
        {
            CompassTextArray.Add(Text);
        }
    }

    // 스크롤 박스 설정
    ScrollBox_Compass->SetAnimateWheelScrolling(false);
    ScrollBox_Compass->SetScrollBarVisibility(ESlateVisibility::Collapsed);

    // 조준점 초기화
    if (Crosshair)
    {
        // 기본적으로 조준점 표시
        Crosshair->SetVisibility(ESlateVisibility::Visible);
    }

    CurrentHpPercent = 1.f;
    CurrentStPercent = 1.f;

    BGHpPercent = 1.f;
    BGStPercent = 1.f;

    // 초기화 완료 플래그 설정
    testcheck = true;
}

void UNS_PlayerHUD::ShowWidget()
{
    // HUD 위젯을 화면에 표시
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
            0.5f,
            false
        );
        return;
    }

    CachedPlayerCharacter = Cast<ANS_PlayerCharacterBase>(MyPawn);
    if (!CachedPlayerCharacter) return;

    // 타이머 핸들 정리 (NativeTick에서 처리하므로 더 이상 필요 없음)
    GetWorld()->GetTimerManager().ClearTimer(UpdatePlayerStausHandle);
}

void UNS_PlayerHUD::HideWidget()
{
    // HUD 위젯을 화면에서 숨김
    SetVisibility(ESlateVisibility::Hidden);
}

void UNS_PlayerHUD::SetYeddaItem(APickup* YeddaItem)
{
    // 이전에는 나침반에 마커를 표시했으나 현재는 사용하지 않음
    UE_LOG(LogTemp, Warning, TEXT("SetYeddaItem called, but marker logic is removed."));
}

void UNS_PlayerHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    // 부모 클래스의 NativeTick 호출
    Super::NativeTick(MyGeometry, InDeltaTime);

    // 나침반 스크롤 박스의 지오메트리 안전성 검사
    const FGeometry& CompassGeo = ScrollBox_Compass->GetCachedGeometry();
    if (CompassGeo.GetLocalSize().X <= 0.f)
    {
        return;
    }

    // 필요한 객체들이 유효한지 검사
    if (!testcheck || !GetOwningPlayer() || !CachedPlayerCharacter || !ScrollBox_Compass || CompassTextArray.Num() < 72)
    {
        return;
    }

    // 플레이어의 현재 방향(Yaw) 가져오기
    const float PlayerYaw = FRotator::NormalizeAxis(GetOwningPlayer()->GetControlRotation().Yaw);
    // 나침반 각 방향 사이의 각도 간격
    const float AngleGap = 15.f;

    // 스크롤링을 위한 Yaw 값 계산 (0~360도 범위로 정규화)
    const float YawForScrolling = FMath::Fmod(PlayerYaw + 360.f, 360.f);
    // 현재 방향에 해당하는 인덱스 계산
    const int32 CurrentIndex = FMath::FloorToInt(YawForScrolling / AngleGap);
    // 보간을 위한 소수 부분 계산
    const float InterpFraction = FMath::Fmod(YawForScrolling / AngleGap, 1.f);
    // 중앙에 표시할 방향 인덱스 계산
    const int32 FinalIndex = CurrentIndex + 24;

    // 인덱스 유효성 검사
    if (!CompassTextArray.IsValidIndex(FinalIndex) || !CompassTextArray.IsValidIndex(FinalIndex + 1))
    {
        return;
    }

    // 나침반 요소 유효성 검사
    if (!IsValid(CompassTextArray[FinalIndex]) || !IsValid(CompassTextArray[FinalIndex + 1]))
    {
        return;
    }

    // 기준 오프셋 계산
    float BaseOffset = 0.f;
    for (int32 i = 0; i < FinalIndex; ++i)
    {
        if (IsValid(CompassTextArray[i]))
        {
            BaseOffset += CompassTextArray[i]->GetCachedGeometry().GetLocalSize().X;
        }
    }

    // 현재 방향과 다음 방향의 너비 가져오기
    const float WidthA = CompassTextArray[FinalIndex]->GetCachedGeometry().GetLocalSize().X;
    const float WidthB = CompassTextArray[FinalIndex + 1]->GetCachedGeometry().GetLocalSize().X;
    // 보간된 오프셋 계산
    const float InterpolatedOffset = ((WidthA / 2.f) + (WidthB / 2.f)) * InterpFraction;

    // 스크롤 박스의 너비 가져오기
    const float ViewWidth = ScrollBox_Compass->GetCachedGeometry().GetLocalSize().X;
    // 최종 스크롤 오프셋 계산
    float TargetOffset = BaseOffset + InterpolatedOffset - (ViewWidth / 2.f) + (WidthA / 2.f);

    // 스크롤 박스 오프셋 설정
    ScrollBox_Compass->SetScrollOffset(TargetOffset);

    // 각 나침반 요소의 스타일 업데이트
    for (int32 i = 0; i < CompassTextArray.Num(); ++i)
    {
        if (UNS_CompassElement* Elem = CompassTextArray.IsValidIndex(i) ? CompassTextArray[i] : nullptr; IsValid(Elem))
        {
            // 마커 하이라이트 기능 비활성화 (항상 false)
            bool bHighlight = false;

            // 텍스트 색상 설정
            if (IsValid(Elem->TextDir))
            {
                Elem->TextDir->SetColorAndOpacity(FSlateColor(bHighlight ? FLinearColor::Red : FLinearColor::White));
            }

            // 화살표 이미지 표시 여부 설정
            if (IsValid(Elem->Image_Arrow))
            {
                Elem->Image_Arrow->SetVisibility(bHighlight ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
            }
        }
    }

    // 플레이어 상태 업데이트 (이전에 타이머로 처리하던 부분을 NativeTick으로 이동)
    // 로컬 플레이어만 UI 업데이트 (멀티플레이어에서 다른 플레이어의 UI는 업데이트하지 않음)
    if (CachedPlayerCharacter && CachedPlayerCharacter->StatusComp && CachedPlayerCharacter->IsLocallyControlled())
    {
        // 목표값 (0으로 나누기 방지)
        const float NewHpPercent = CachedPlayerCharacter->StatusComp->MaxHealth > 0.f ?
            CachedPlayerCharacter->StatusComp->Health / CachedPlayerCharacter->StatusComp->MaxHealth : 0.f;
        const float NewStPercent = CachedPlayerCharacter->StatusComp->MaxStamina > 0.f ?
            CachedPlayerCharacter->StatusComp->Stamina / CachedPlayerCharacter->StatusComp->MaxStamina : 0.f;
        const bool bIsCurrentlySprint = CachedPlayerCharacter->IsSprint;

        // 체력 바 업데이트
        if (ProgressBar_Health)
        {
            CurrentHpPercent = FMath::Lerp(CurrentHpPercent, NewHpPercent, 0.2f);
            ProgressBar_Health->SetPercent(CurrentHpPercent);
        }

        // if (ProgressBarBG_Health)
        // {
        //     BGHpPercent = FMath::Lerp(BGHpPercent, NewHpPercent, 0.05f);
        //     ProgressBarBG_Health->SetPercent(BGHpPercent);
        // }

        // 스태미너 바 업데이트
        if (ProgressBar_Stamina)
        {
            if (bIsCurrentlySprint) // 현재 스프린트 중일 때
            {
                ProgressBar_Stamina->SetPercent(NewStPercent);
                // 스프린트 중에는 배경 바를 현재 스태미너와 동일하게 유지하거나, 빠르게 따라가게 할 수 있습니다.
                // 여기서는 현재 스태미너와 동일하게 설정합니다.
                // ProgressBarBG_Stamina->SetPercent(NewStPercent);
                CurrentStPercent = NewStPercent; // 보간 없이 즉시 적용
                // BGStPercent = NewStPercent; // 보간 없이 즉시 적용
            }
            else // 현재 대쉬 중이 아닐 때 (스태미너 회복 또는 일반 상태)
            {
                CurrentStPercent = FMath::Lerp(CurrentStPercent, NewStPercent, 0.2f);
                ProgressBar_Stamina->SetPercent(CurrentStPercent);
                
                // // 스태미너 배경 바는 현재 스태미너보다 뒤쳐지게 (즉, 감소 효과) 또는 부드럽게 따라가게 합니다.
                // // NewStPercent는 항상 목표값이므로, Lerp를 사용하여 부드럽게 업데이트합니다.
                // BGStPercent = FMath::Lerp(BGStPercent, NewStPercent, 0.05f);
                // ProgressBarBG_Stamina->SetPercent(BGStPercent);
            }
        }

        // 조준 상태에 따라 조준점 표시/숨김 처리
        if (Crosshair)
        {
            // 캐릭터가 조준 중이면 조준점 숨김
            bool bIsAiming = CachedPlayerCharacter->IsAimingChange();
            SetCrosshairVisibility(!bIsAiming);
        }
    }
}

void UNS_PlayerHUD::DeleteCompasItem(APickup* DeleteItem)
{
    // 나침반 아이템 삭제 기능 (현재 사용하지 않음)
    return;
}

void UNS_PlayerHUD::SetTipText(const FText& NewText)
{
    // 팁 텍스트 설정
    if (IsValid(TipText))
    {
        TipText->SetText(NewText);
    }
}

void UNS_PlayerHUD::SetCrosshairVisibility(bool bVisible)
{
    if (Crosshair)
    {
        Crosshair->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
    }
}

// TipText삭제용 함수 === 쪽지 주우면 사라지도록 하기 위함
void UNS_PlayerHUD::HideTipText()
{
    if (IsValid(TipText))
    {
        TipText->SetVisibility(ESlateVisibility::Hidden);
    }
}