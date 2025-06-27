#include "GameFlow/NS_LobbyMode.h"
#include "NS_PlayerState.h"
#include "EngineUtils.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "GameFlow/NS_GameInstance.h" 
#include "GameFramework/GameState.h"
#include "NS_LobbyController.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "UI/NS_UIManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerStart.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"

/**
 * 생성자 - 기본 설정 초기화
 */
ANS_LobbyMode::ANS_LobbyMode()
{
    // 기본 폰 클래스를 null로 설정 (PostLogin에서 직접 생성)
    DefaultPawnClass = nullptr; 
}

/**
 * 게임 시작 시 호출 - 온라인 세션 설정
 */
void ANS_LobbyMode::BeginPlay()
{
    Super::BeginPlay();

    // 온라인 서브시스템 가져오기
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (!Subsystem) return;

    // 세션 인터페이스 가져오기
    IOnlineSessionPtr Sessions = Subsystem->GetSessionInterface();
    if (!Sessions.IsValid()) return;

    // 세션 설정
    FOnlineSessionSettings SessionSettings;
    SessionSettings.bIsDedicated = true;        // 전용 서버 설정
    SessionSettings.bIsLANMatch = false;        // 인터넷 게임 설정
    SessionSettings.NumPublicConnections = 4;   // 최대 4명 접속 가능
    SessionSettings.bShouldAdvertise = true;    // 세션 공개 설정
    SessionSettings.bUsesPresence = true;       // 프레즌스 기능 사용
    SessionSettings.bAllowJoinInProgress = true; // 게임 중 참가 허용

    // 세션 생성
    Sessions->CreateSession(0, NAME_GameSession, SessionSettings);

    UE_LOG(LogTemp, Warning, TEXT("Dedicated Server에서 CreateSession 호출됨"));
}

/**
 * 플레이어 로그인 시 호출 - 캐릭터 생성 및 초기화
 * @param NewPlayer 새로 접속한 플레이어 컨트롤러
 */
void ANS_LobbyMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    // 게임 인스턴스 가져오기
    UNS_GameInstance* GI = GetGameInstance<UNS_GameInstance>();
    if (!GI)
    {
        UE_LOG(LogTemp, Error, TEXT("UNS_GameInstance를 찾을 수 없습니다!"));
        return;
    }

    // 플레이어 인덱스 계산 (접속 순서대로 0, 1, 2, 3)
    int32 PlayerIndex = GetGameState<AGameState>()->PlayerArray.Num() - 1;

    // 유효한 캐릭터 클래스 확인
    if (!GI->AvailableCharacterClasses.IsValidIndex(PlayerIndex))
    {
        return;
    }

    // 스폰 위치 찾기
    AActor* StartSpot = FindSpawnPointByIndex(PlayerIndex);
    if (!StartSpot)
    {
        return;
    }

    // 스폰 위치 및 회전 설정
    FTransform SpawnTransform = StartSpot->GetActorTransform();

    // 캐릭터 생성
    APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(GI->AvailableCharacterClasses[PlayerIndex], SpawnTransform);
    if (SpawnedPawn)
    {
        // 플레이어 컨트롤러에 폰 할당
        NewPlayer->Possess(SpawnedPawn);
        UE_LOG(LogTemp, Log, TEXT("Spawned & Possessed Pawn: %s from GameInstance"), *SpawnedPawn->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Pawn spawn failed."));
    }

    // 플레이어 상태 설정
    if (ANS_PlayerState* PS = Cast<ANS_PlayerState>(NewPlayer->PlayerState))
    {
        PS->SetPlayerIndex(PlayerIndex);
        PS->SavePlayerData();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerState cast to ANS_PlayerState failed."));
    }

    // 모든 플레이어의 UI 업데이트
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (ANS_LobbyController* LC = Cast<ANS_LobbyController>(*It))
        {
            if (UNS_GameInstance* GameInst = Cast<UNS_GameInstance>(GetGameInstance()))
            {
                if (GameInst->ReadyUIInstance)
                {
                    GameInst->ReadyUIInstance->UpdatePlayerStatusList();
                }
            }
        }
    }
}

/**
 * 플레이어 로그아웃 시 호출 - 플레이어 정리 및 UI 업데이트
 * @param Exiting 로그아웃하는 컨트롤러
 */
void ANS_LobbyMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);

    // 플레이어 컨트롤러 확인
    APlayerController* PlayerController = Cast<APlayerController>(Exiting);
    if (!PlayerController) return;

    // 플레이어 상태 로그 출력
    if (ANS_PlayerState* PS = Cast<ANS_PlayerState>(PlayerController->PlayerState))
    {
        UE_LOG(LogTemp, Log, TEXT("Player %s with index %d has left the lobby."), *PS->GetPlayerName(), PS->PlayerIndex);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to cast PlayerState to ANS_PlayerState during logout."));
    }

    // 남은 플레이어들의 UI 업데이트
    if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        if (GI->ReadyUIInstance)
        {
            GI->ReadyUIInstance->UpdatePlayerStatusList();
        }
    }
}

/**
 * 인덱스에 해당하는 스폰 포인트 찾기
 * @param Index 플레이어 인덱스
 * @return 스폰 포인트 액터 (없으면 nullptr)
 */
AActor* ANS_LobbyMode::FindSpawnPointByIndex(int32 Index)
{
    // 월드의 모든 PlayerStart 액터를 순회
    int32 CurrentIndex = 0;
    for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
    {
        if (CurrentIndex == Index)
        {
            return *It;
        }
        ++CurrentIndex;
    }
    return nullptr;
}

/**
 * 모든 플레이어의 준비 상태 확인 및 게임 시작 처리
 * 모든 플레이어가 준비 완료되면 게임 레벨로 이동
 */
void ANS_LobbyMode::CheckAllPlayersReady()
{
    // 게임 상태 가져오기
    const AGameStateBase* GS = GetGameState<AGameStateBase>();
    if (!GS) return;

    //// 플레이어 수 체크 (주석 처리됨)
    //if (GS->PlayerArray.Num() < 2)
    //{
    //	UE_LOG(LogTemp, Warning, TEXT("플레이어 수 부족. 현재 인원: %d"), GS->PlayerArray.Num());
    //	return;
    //}

    // 전원 레디 확인
    for (APlayerState* PS : GS->PlayerArray)
    {
        if (ANS_PlayerState* MyPS = Cast<ANS_PlayerState>(PS))
        {
            if (!MyPS->GetIsReady())
            {
                UE_LOG(LogTemp, Warning, TEXT("아직 준비 안된 플레이어 있음"));
                return;
            }
        }
    }

    // 모든 플레이어에게 대기 UI 표시
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (APlayerController* PC = It->Get())
        {
            if (ANS_LobbyController* LC = Cast<ANS_LobbyController>(PC))
            {
                LC->Client_ShowWait();
            }
        }
    }

    // 다음 틱에 타이머 설정 (약간의 지연 후 레벨 이동)
    GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
    {
        FTimerHandle DelayHandle;
        GetWorld()->GetTimerManager().SetTimer(DelayHandle, [this]()
        {
            UE_LOG(LogTemp, Warning, TEXT("ServerTravel 실행: MainWorld 이동"));
            const FString LevelPath = TEXT("/Game/Maps/MainWorld");
            const FString Options = TEXT("Game=/Game/GameFlowBP/BP_NS_MultiPlayMode.BP_NS_MultiPlayMode_C");
            GetWorld()->ServerTravel(LevelPath + TEXT("?") + Options);
        }, 1.0f, false);
    });
}