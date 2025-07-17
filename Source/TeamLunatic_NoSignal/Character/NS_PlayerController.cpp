#include "Character/NS_PlayerController.h"
#include "GameFlow/NS_GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HUD/NS_InGameHUD.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Character/Components/NS_StatusComponent.h"

/**
 * 플레이어 컨트롤러 생성자
 * 기본 설정을 초기화합니다.
 */
ANS_PlayerController::ANS_PlayerController()
{
    // 마우스 커서 초기 설정 (기본적으로 숨김, 인게임 메뉴에서만 표시)
    bShowMouseCursor = false;
    
    // 클릭 이벤트 활성화 (UI 요소와 상호작용 가능)
    bEnableClickEvents = true;
    
    // 마우스 오버 이벤트 활성화 (UI 요소에 마우스를 올렸을 때 반응)
    bEnableMouseOverEvents = true;
}

/**
 * 게임 시작 시 호출되는 함수
 * 플레이어 HUD를 초기화하고 UI 매니저에 등록합니다.
 */
void ANS_PlayerController::BeginPlay()
{
    // 부모 클래스의 BeginPlay 호출
    Super::BeginPlay();
}

/**
 * 추적 사운드 재생 함수
 * 플레이어가 적에게 추적당할 때 사운드를 재생합니다.
 * 블루프린트에서 구현 가능한 네이티브 이벤트입니다.
 */
void ANS_PlayerController::PlayTracked_Implementation()
{
    // 추적 시작 사운드 재생
    UGameplayStatics::PlaySound2D(this, ChaseStartSoundCue);
    UE_LOG(LogTemp, Error, TEXT("소리재생"));
}

/**
 * 게임 오버 처리 함수
 * 플레이어가 사망하거나 게임이 종료될 때 호출됩니다.
 * @param bPlayerSurvived - 플레이어 생존 여부 (true: 생존, false: 사망)
 */
void ANS_PlayerController::HandleGameOver(bool bPlayerSurvived)
{
    UE_LOG(LogTemp, Warning, TEXT(">> HandleGameOver 진입 (bPlayerSurvived: %s)"), bPlayerSurvived ? TEXT("true") : TEXT("false"));

    // 입력 무시 설정
    SetIgnoreMoveInput(true);
    SetIgnoreLookInput(true);
    
    // 마우스 커서 표시
    bShowMouseCursor = true;

    // UI 모드로 전환
    FInputModeUIOnly InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(InputMode);

    // 싱글플레이어 모드에서는 게임 시간 정지
    if (GetWorld()->GetNumPlayerControllers() == 1)
    {
        UE_LOG(LogTemp, Warning, TEXT(">> 싱글플레이 - 글로벌 시간 정지"));
        UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.f);
    }
}