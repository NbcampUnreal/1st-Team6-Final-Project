// NS_ReadyUI.cpp
#include "NS_ReadyUI.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "NS_GameInstance.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "NS_PlayerState.h"

// 위젯이 생성될 때 호출되는 함수
void UNS_ReadyUI::NativeConstruct()
{
    Super::NativeConstruct();

    // Ready 버튼에 클릭 이벤트 바인딩
    if (ReadyButton)
    {
        ReadyButton->OnClicked.AddDynamic(this, &UNS_ReadyUI::OnReadyButtonClicked);
    }

    // Quit 버튼에 클릭 이벤트 바인딩
    if (QuitButton)
    {
        QuitButton->OnClicked.AddDynamic(this, &UNS_ReadyUI::OnQuitButtonClicked);
    }
}

// 매 프레임마다 호출되는 함수
void UNS_ReadyUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    UpdatePlayerStatusList(); // 플레이어 상태 목록 실시간 갱신
}

// Ready 버튼 클릭 시 호출되는 함수
void UNS_ReadyUI::OnReadyButtonClicked()
{
    // 현재 플레이어 컨트롤러 가져오기
    APlayerController* PC = GetOwningPlayer();
    if (!PC) return;
    
    // 플레이어 상태 가져오기
    if (ANS_PlayerState* PS = Cast<ANS_PlayerState>(PC->PlayerState))
    {
        // 현재 Ready 상태의 반대로 설정
        bIsPlayerReady = !PS->GetIsReady();
        // 서버에 Ready 상태 변경 요청
        PS->ServerSetIsReady(bIsPlayerReady);
        
        // 자신의 상태 텍스트 즉시 업데이트
        int32 MyIndex = PS->PlayerIndex;
        if (MyIndex >= 0 && MyIndex < 4)
        {
            TArray<UTextBlock*> StatusBlocks = { Text_Status0, Text_Status1, Text_Status2, Text_Status3 };
            if (StatusBlocks[MyIndex])
            {
                StatusBlocks[MyIndex]->SetText(FText::FromString(bIsPlayerReady ? TEXT("Ready") : TEXT("Not Ready")));
            }
        }
    }
}

// Quit 버튼 클릭 시 호출되는 함수
void UNS_ReadyUI::OnQuitButtonClicked()
{
    // 현재 플레이어 컨트롤러 가져오기
    APlayerController* PC = GetOwningPlayer();
    if (!PC) return;

    if (ANS_PlayerState* PS = Cast<ANS_PlayerState>(PC->PlayerState))
    {
        int32 MyIndex = PS->PlayerIndex;

        if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
        {
            // 호스트(인덱스 0)인 경우 세션 파괴
            if (MyIndex == 0)
            {
                GI->DestroyCurrentSession();
            }
            else
            {
                // 게스트인 경우 로그 출력
                UE_LOG(LogTemp, Warning, TEXT("[ReadyUI] Guest가 세션에서 이탈합니다 (Index: %d)"), MyIndex);
            }
        }

        // 메인 타이틀 화면으로 이동
        PC->ClientTravel(TEXT("/Game/UI/Map//MainTitle"), ETravelType::TRAVEL_Absolute);
    }
}

// 플레이어 상태 목록 업데이트 함수 - 최적화 버전
void UNS_ReadyUI::UpdatePlayerStatusList()
{
    // 월드 객체 가져오기
    UWorld* World = GetWorld();
    if (!World) return;

    // 게임 상태 가져오기
    AGameStateBase* GameState = World->GetGameState();
    if (!GameState) return;

    // 플레이어 배열 복사
    TArray<APlayerState*> SortedPlayers = GameState->PlayerArray;

    // PlayerIndex 기준으로 플레이어 정렬
    SortedPlayers.Sort([](const APlayerState& A, const APlayerState& B)
    {
        const ANS_PlayerState* PSA = Cast<const ANS_PlayerState>(&A);
        const ANS_PlayerState* PSB = Cast<const ANS_PlayerState>(&B);
        return (PSA && PSB) ? PSA->PlayerIndex < PSB->PlayerIndex : false;
    });

    // UI 요소 배열 설정
    TArray<UTextBlock*> NameBlocks = { Text_Player0, Text_Player1, Text_Player2, Text_Player3 };
    TArray<UTextBlock*> StatusBlocks = { Text_Status0, Text_Status1, Text_Status2, Text_Status3 };

    // 현재 플레이어의 인덱스 구하기
    int32 MyIndex = -1;
    if (APlayerController* PC = GetOwningPlayer())
    {
        if (ANS_PlayerState* MyPS = Cast<ANS_PlayerState>(PC->PlayerState))
        {
            MyIndex = MyPS->PlayerIndex;
            // 자신의 준비 상태 업데이트
            bIsPlayerReady = MyPS->GetIsReady();
        }
    }

    // 각 플레이어 슬롯 업데이트
    for (int32 i = 0; i < NameBlocks.Num(); ++i)
    {
        if (!NameBlocks[i] || !StatusBlocks[i]) continue;

        // 플레이어가 있는 슬롯
        if (i < SortedPlayers.Num())
        {
            if (ANS_PlayerState* PS = Cast<ANS_PlayerState>(SortedPlayers[i]))
            {
                // 플레이어 이름 표시
                NameBlocks[i]->SetText(FText::FromString(FString::Printf(TEXT("Player %d"), PS->PlayerIndex + 1)));
                
                // 플레이어 상태 표시 (자신의 상태는 이미 버튼 클릭 시 업데이트됨)
                if (PS->PlayerIndex != MyIndex)
                {
                    StatusBlocks[i]->SetText(FText::FromString(PS->GetIsReady() ? TEXT("Ready") : TEXT("Not Ready")));
                }

                // UI 요소 표시
                NameBlocks[i]->SetVisibility(ESlateVisibility::Visible);
                StatusBlocks[i]->SetVisibility(ESlateVisibility::Visible);

            }
        }
        else
        {
            // 빈 슬롯 표시 (변경 없음)
            NameBlocks[i]->SetText(FText::FromString(FString::Printf(TEXT("Empty Slot %d"), i + 1)));
            StatusBlocks[i]->SetText(FText::FromString(TEXT("-")));
            NameBlocks[i]->SetVisibility(ESlateVisibility::Visible);
            StatusBlocks[i]->SetVisibility(ESlateVisibility::Visible);
        }
    }
}