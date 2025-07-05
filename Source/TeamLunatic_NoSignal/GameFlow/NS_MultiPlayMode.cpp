#include "NS_MultiPlayMode.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "NS_GameInstance.h"
#include "NS_MainGamePlayerState.h"
#include "NS_GameState.h"
#include "NS_LobbyController.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "GameFramework/PlayerStart.h"
#include "Zombie/NS_ZombieBase.h"
#include "Algo/RandomShuffle.h"
#include "Character/NS_PlayerController.h"
#include "Engine/World.h"

ANS_MultiPlayMode::ANS_MultiPlayMode()
{
}

void ANS_MultiPlayMode::BeginPlay()
{
    UsedPawnIndices.Empty();
    // 부모 클래스의 BeginPlay 호출 전에 플레이어 수 계산
    if (const ANS_GameState* GS = GetGameState<ANS_GameState>())
    {
        PlayerCount = 0;
        for (APlayerState* PS : GS->PlayerArray)
        {
            if (ANS_MainGamePlayerState* MPS = Cast<ANS_MainGamePlayerState>(PS))
            {
                if (MPS->bIsAlive)
                {
                    PlayerCount++;
                }
            }
        }

        // 최소 1명 이상으로 설정
        PlayerCount = FMath::Max(1, PlayerCount);
    }

    if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        GI->SetGameModeType(EGameModeType::MultiPlayMode);

        if (IsRunningDedicatedServer())
        {
            if (GI->MyServerPort > 0)
            {
                GI->RequestUpdateSessionStatus(GI->MyServerPort, TEXT("in_game"));
            }
        }
    }
}


void ANS_MultiPlayMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    if (NewPlayer)
    {
        if (MainGamePawnClassesToSpawn.Num() == 0)
        {
            return;
        }

        // 중복되지 않는 폰 클래스를 선택합니다.
        TSubclassOf<APawn> ChosenPawnClass = GetUniqueRandomPawnClass();

        // 모든 PlayerStart를 찾아서 랜덤으로 하나를 선택합니다.
        TArray<AActor*> PlayerStarts;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

        AActor* StartSpot = nullptr;
        if (PlayerStarts.Num() > 0)
        {
            const int32 RandStartSpotIndex = FMath::RandRange(0, PlayerStarts.Num() - 1);
            StartSpot = PlayerStarts[RandStartSpotIndex];
        }

        if (!StartSpot)
        {
            return;
        }

        if (ChosenPawnClass)
        {
            FActorSpawnParameters SpawnInfo;
            SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

            APawn* NewPawn = GetWorld()->SpawnActor<APawn>(ChosenPawnClass, StartSpot->GetActorLocation(), StartSpot->GetActorRotation(), SpawnInfo);

            if (NewPawn)
            {
                NewPlayer->Possess(NewPawn);
                int32 SelectedIndex = MainGamePawnClassesToSpawn.IndexOfByKey(ChosenPawnClass);
                if (SelectedIndex != INDEX_NONE)
                {
                    // 플레이어 컨트롤러에 사용된 인덱스 정보 저장 (커스텀 프로퍼티 필요)
                    if (ANS_PlayerController* PC = Cast<ANS_PlayerController>(NewPlayer))
                    {
                        // PlayerController에 사용된 폰 인덱스 저장하는 변수가 필요함
                        // 임시로 로그만 출력
                        UE_LOG(LogTemp, Log, TEXT("Player %s assigned pawn index %d"), *NewPlayer->PlayerState->GetPlayerName(), SelectedIndex);
                    }
                }

                UE_LOG(LogTemp, Log, TEXT("Player %s spawned and possessed unique pawn %s (Index: %d)."),
                    *NewPlayer->PlayerState->GetPlayerName(), *NewPawn->GetName(), SelectedIndex);
                NotifyPlayerLogin();
                // Flask 서버에 플레이어 로그인 알림
            }
        }
    }
}



FVector ANS_MultiPlayMode::GetPlayerLocation_Implementation() const
{
    if (const ANS_GameState* GS = GetGameState<ANS_GameState>())
    {
        if (APawn* Target = GS->TrackingTarget)
        {
            if (AController* Controller = Target->GetController())
            {
                if (ANS_MainGamePlayerState* PS = Controller->GetPlayerState<ANS_MainGamePlayerState>())
                {
                    if (PS->bIsAlive)
                    {
                        return Target->GetActorLocation();
                    }
                }
            }
        }

        for (APlayerState* PS : GS->PlayerArray)
        {
            if (ANS_MainGamePlayerState* MPS = Cast<ANS_MainGamePlayerState>(PS))
            {
                if (MPS->bIsAlive)
                {
                    if (APawn* AlivePawn = MPS->GetPawn())
                    {
                        return AlivePawn->GetActorLocation();
                    }
                }
            }
        }
    }

    return FVector::ZeroVector;
}

void ANS_MultiPlayMode::OnPlayerCharacterDied_Implementation(ANS_PlayerCharacterBase* DeadCharacter)
{
    if (!HasAuthority()) return;

    if (AController* Controller = DeadCharacter->GetController())
    {
        if (ANS_MainGamePlayerState* PS = Controller->GetPlayerState<ANS_MainGamePlayerState>())
        {
            PS->bIsAlive = false;
        }
    }
}

void ANS_MultiPlayMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);

    if (!HasAuthority()) return;

    if (APlayerController* PC = Cast<APlayerController>(Exiting))
    {
        ReleasePawnIndex(PC);
    }
    NotifyPlayerLogout();
    // Flask 서버에 플레이어 로그아웃 알림
    // 플레이어 로그아웃 후 플레이어 수 체크
    CheckPlayerCountAndEndSession();
}

// 주기적으로 플레이어 수를 체크하고 세션을 종료하는 함수 구현
void ANS_MultiPlayMode::CheckPlayerCountAndEndSession()
{
    // 서버가 아니면 실행하지 않음
    if (!HasAuthority())
    {
        return;
    }

    int32 ConnectedPlayerCount = GetNumPlayers();

    if (ConnectedPlayerCount == 0)
    {
        if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
        {
            if (GI->MyServerPort > 0)
            {
                GI->RequestUpdateSessionStatus(GI->MyServerPort, TEXT("closed"));
            }
        }
    }
}

// Flask 서버에 플레이어 로그인 알림
void ANS_MultiPlayMode::NotifyPlayerLogin()
{
    if (!HasAuthority()) return;

    if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        if (GI->MyServerPort > 0)
        {
            // HTTP 요청으로 플레이어 로그인 알림
            FHttpModule& HttpModule = FHttpModule::Get();
            TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = HttpModule.CreateRequest();

            Request->SetURL(TEXT("http://121.163.249.108:5000/player_login"));
            Request->SetVerb(TEXT("POST"));
            Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

            // JSON 데이터 생성
            TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
            JsonObject->SetNumberField(TEXT("port"), GI->MyServerPort);

            FString OutputString;
            TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
            FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

            Request->SetContentAsString(OutputString);

            Request->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
            {
                if (bWasSuccessful && Response.IsValid())
                {
                    UE_LOG(LogTemp, Log, TEXT("[NotifyPlayerLogin] 플레이어 로그인 알림 성공"));
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("[NotifyPlayerLogin] 플레이어 로그인 알림 실패"));
                }
            });

            Request->ProcessRequest();
        }
    }
}

// Flask 서버에 플레이어 로그아웃 알림
void ANS_MultiPlayMode::NotifyPlayerLogout()
{
    if (!HasAuthority()) return;

    if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        if (GI->MyServerPort > 0)
        {
            // HTTP 요청으로 플레이어 로그아웃 알림
            FHttpModule& HttpModule = FHttpModule::Get();
            TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = HttpModule.CreateRequest();

            Request->SetURL(TEXT("http://121.163.249.108:5000/player_logout"));
            Request->SetVerb(TEXT("POST"));
            Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

            // JSON 데이터 생성
            TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
            JsonObject->SetNumberField(TEXT("port"), GI->MyServerPort);

            FString OutputString;
            TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
            FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

            Request->SetContentAsString(OutputString);

            Request->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
            {
                if (bWasSuccessful && Response.IsValid())
                {
                    UE_LOG(LogTemp, Log, TEXT("[NotifyPlayerLogout] 플레이어 로그아웃 알림 성공"));
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("[NotifyPlayerLogout] 플레이어 로그아웃 알림 실패"));
                }
            });

            Request->ProcessRequest();
        }
    }
}

void ANS_MultiPlayMode::OnPlayerLoadingComplete(APlayerController* Player)
{
    if (!HasAuthority()) return;

    // 이미 완료된 플레이어인지 확인
    if (LoadingCompletedPlayers.Contains(Player))
    {
        return;
    }

    // 완료된 플레이어 목록에 추가
    LoadingCompletedPlayers.Add(Player);
    // 모든 플레이어가 완료되었는지 확인
    CheckAllPlayersLoadingComplete();
}

void ANS_MultiPlayMode::CheckAllPlayersLoadingComplete()
{
    if (!HasAuthority()) return;

    int32 TotalPlayers = GetNumPlayers();
    int32 CompletedPlayers = LoadingCompletedPlayers.Num();

    // 모든 플레이어가 로딩 완료되었으면
    if (CompletedPlayers >= TotalPlayers && TotalPlayers > 0)
    {
        // 모든 클라이언트에게 로딩 스크린 숨기기 명령
        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            if (APlayerController* PC = It->Get())
            {
                if (ANS_LobbyController* LC = Cast<ANS_LobbyController>(PC))
                {
                    LC->Client_HideLoadingScreen();
                }
            }
        }

        // 완료된 플레이어 목록 초기화
        LoadingCompletedPlayers.Empty();
    }
}

// 랜덤한 살아있는 플레이어의 위치를 반환하는 함수
FVector ANS_MultiPlayMode::GetRandomPlayerLocation() const
{
    TArray<FVector> PlayerLocations;

    if (const ANS_GameState* GS = GetGameState<ANS_GameState>())
    {
        // 게임 스테이트의 모든 플레이어 상태를 순회하여 살아있는 플레이어 위치 수집
        for (APlayerState* PS : GS->PlayerArray)
        {
            if (ANS_MainGamePlayerState* MPS = Cast<ANS_MainGamePlayerState>(PS))
            {
                if (MPS->bIsAlive)
                {
                    if (APawn* AlivePawn = MPS->GetPawn())
                    {
                        PlayerLocations.Add(AlivePawn->GetActorLocation());
                    }
                }
            }
        }
    }

    // 살아있는 플레이어가 있으면 랜덤하게 한 명 선택
    if (PlayerLocations.Num() > 0)
    {
        int32 RandomIndex = FMath::RandRange(0, PlayerLocations.Num() - 1);
        return PlayerLocations[RandomIndex];
    }

    // 살아있는 플레이어가 없으면 기본 위치 반환
    return FVector::ZeroVector;
}


// 최적화된 멀티플레이 스폰 체크
void ANS_MultiPlayMode::OptimizedMultiplaySpawnCheck()
{
    // 서버 부하 체크 - 프레임 시간이 33ms(30FPS) 이상이면 건너뜀
    if (GetWorld()->GetDeltaSeconds() < 0.033f)
    {
        return;
    }

    // 게임 오버 상태면 스폰 중단
    if (bIsGameOver)
    {
        return;
    }
}

// 중복되지 않는 폰 클래스 선택 함수
TSubclassOf<APawn> ANS_MultiPlayMode::GetUniqueRandomPawnClass()
{
    if (MainGamePawnClassesToSpawn.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("[GetUniqueRandomPawnClass] MainGamePawnClassesToSpawn 배열이 비어있습니다."));
        return nullptr;
    }

    // 사용 가능한 인덱스들을 찾습니다
    TArray<int32> AvailableIndices;
    for (int32 i = 0; i < MainGamePawnClassesToSpawn.Num(); i++)
    {
        if (!UsedPawnIndices.Contains(i))
        {
            AvailableIndices.Add(i);
        }
    }

    // 모든 캐릭터가 사용 중인 경우
    if (AvailableIndices.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[GetUniqueRandomPawnClass] 모든 캐릭터가 사용 중입니다. 첫 번째 캐릭터를 중복 할당합니다."));

        // 모든 캐릭터가 사용 중이면 첫 번째 캐릭터를 반환 (또는 랜덤 선택)
        int32 FallbackIndex = FMath::RandRange(0, MainGamePawnClassesToSpawn.Num() - 1);
        UsedPawnIndices.Add(FallbackIndex);
        return MainGamePawnClassesToSpawn[FallbackIndex];
    }

    // 사용 가능한 인덱스 중에서 랜덤 선택
    int32 RandomAvailableIndex = AvailableIndices[FMath::RandRange(0, AvailableIndices.Num() - 1)];

    // 선택된 인덱스를 사용 중 목록에 추가
    UsedPawnIndices.Add(RandomAvailableIndex);

    UE_LOG(LogTemp, Log, TEXT("[GetUniqueRandomPawnClass] 선택된 폰 인덱스: %d, 사용 중인 인덱스 수: %d/%d"),
        RandomAvailableIndex, UsedPawnIndices.Num(), MainGamePawnClassesToSpawn.Num());

    return MainGamePawnClassesToSpawn[RandomAvailableIndex];
}

// 플레이어 로그아웃 시 사용된 폰 인덱스 해제
void ANS_MultiPlayMode::ReleasePawnIndex(APlayerController* ExitingPlayer)
{
    if (!ExitingPlayer || !ExitingPlayer->GetPawn())
    {
        return;
    }

    // 플레이어가 사용하던 폰 클래스를 찾아서 인덱스 해제
    TSubclassOf<APawn> ExitingPawnClass = ExitingPlayer->GetPawn()->GetClass();
    int32 PawnIndex = MainGamePawnClassesToSpawn.IndexOfByKey(ExitingPawnClass);

    if (PawnIndex != INDEX_NONE && UsedPawnIndices.Contains(PawnIndex))
    {
        UsedPawnIndices.Remove(PawnIndex);
        UE_LOG(LogTemp, Log, TEXT("[ReleasePawnIndex] 폰 인덱스 %d 해제됨. 플레이어: %s, 남은 사용 중인 인덱스 수: %d"),
            PawnIndex, *ExitingPlayer->PlayerState->GetPlayerName(), UsedPawnIndices.Num());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[ReleasePawnIndex] 해제할 폰 인덱스를 찾을 수 없습니다. 플레이어: %s"),
            ExitingPlayer->PlayerState ? *ExitingPlayer->PlayerState->GetPlayerName() : TEXT("Unknown"));
    }
}

// 캐릭터 중복 방지 시스템 리셋 (게임 재시작 시 사용)
void ANS_MultiPlayMode::ResetCharacterDuplicationSystem()
{
    UsedPawnIndices.Empty();
    UE_LOG(LogTemp, Log, TEXT("[ResetCharacterDuplicationSystem] 캐릭터 중복 방지 시스템이 리셋되었습니다. 사용 가능한 캐릭터 수: %d"),
        MainGamePawnClassesToSpawn.Num());
}