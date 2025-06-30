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
#include "Engine/World.h"

ANS_MultiPlayMode::ANS_MultiPlayMode()
{
    // 멀티플레이 모드에서는 2초마다 스폰하도록 설정
    ZombieSpawnInterval = 2.0f;
    // 좀비 스폰 수
    ZombiesPerSpawn = 2;
}

void ANS_MultiPlayMode::BeginPlay()
{
    UE_LOG(LogTemp, Warning, TEXT("MultiPlayMode Set !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
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

        // 플레이어 수에 따라 한 번에 스폰할 좀비 수 설정 (플레이어 수 × 기본 스폰 수)
        ZombiesPerSpawn = PlayerCount * ZombiesPerSpawn;

    }

    // 부모 클래스의 BeginPlay 호출 전에 최대 좀비 수 저장
    int32 BaseMaxZombieCount = MaxZombieCount;

    // 부모 클래스의 BeginPlay 호출 (여기서 MaxZombieCount가 설정됨)
    ANS_GameModeBase::BeginPlay();

    // 플레이어 수에 따라 최대 좀비 수 조정 (플레이어 수 × 기본 최대 좀비 수 ÷ 1.5)
    // 너무 많은 좀비가 생성되지 않도록 1.5로 나눔
    float AdjustedMaxZombies = (BaseMaxZombieCount * PlayerCount) / 1.5f;
    MaxZombieCount = FMath::CeilToInt(AdjustedMaxZombies); // 올림 처리하여 정수로 변환

   /* UE_LOG(LogTemp, Warning, TEXT("[MultiPlayMode] 플레이어 수에 따라 최대 좀비 수 조정: %d (기본: %d × 플레이어 수: %d ÷ 1.5)"),
        MaxZombieCount, BaseMaxZombieCount, PlayerCount);*/

    // 기존 MultiPlayMode의 BeginPlay 로직
    //UE_LOG(LogTemp, Warning, TEXT("MultiPlayMode BeginPlay 실행"));


    if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        GI->SetGameModeType(EGameModeType::MultiPlayMode);

        if (IsRunningDedicatedServer())
        {
            if (GI->MyServerPort > 0)
            {
                GI->RequestUpdateSessionStatus(GI->MyServerPort, TEXT("in_game"));
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("[MultiPlayMode] MyServerPort가 유효하지 않아 매치메이킹 서버에 세션 상태 업데이트 요청을 보낼 수 없습니다."));
            }
        }
    }

    // 멀티플레이 최적화된 타이머 설정
    UE_LOG(LogTemp, Warning, TEXT("[MultiPlayMode] 최적화된 타이머 시스템 활성화"));

    // 플레이어 수가 적을 때만 타이머 활성화 (4명 이하)
    if (PlayerCount <= 4)
    {
        // 안전한 주기로 타이머 설정 (최소 3초 보장)
        float SafeSpawnInterval = FMath::Max(ZombieSpawnInterval, 3.0f);
        GetWorldTimerManager().ClearTimer(ZombieSpawnTimer);
        GetWorldTimerManager().SetTimer(ZombieSpawnTimer, this,
            &ANS_MultiPlayMode::OptimizedMultiplaySpawnCheck, SafeSpawnInterval, true);

        UE_LOG(LogTemp, Warning, TEXT("[MultiPlayMode] 안전 모드 타이머 활성화 (%.1f초 주기, %d마리씩)"),
            SafeSpawnInterval, ZombiesPerSpawn);
    }
    else
    {
        // 플레이어가 많으면 더 긴 주기로 설정
        float ExtendedSpawnInterval = ZombieSpawnInterval * 2.0f; // 4초로 증가
        GetWorldTimerManager().ClearTimer(ZombieSpawnTimer);
        GetWorldTimerManager().SetTimer(ZombieSpawnTimer, this,
            &ANS_MultiPlayMode::OptimizedMultiplaySpawnCheck, ExtendedSpawnInterval, true);

        UE_LOG(LogTemp, Warning, TEXT("[MultiPlayMode] 확장 모드 타이머 활성화 (%.1f초 주기, 플레이어 %d명)"),
            ExtendedSpawnInterval, PlayerCount);
    }
}


void ANS_MultiPlayMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    if (NewPlayer)
    {
        if (MainGamePawnClassesToSpawn.Num() == 0)
        {
            UE_LOG(LogTemp, Error, TEXT("MainGamePawnClassesToSpawn 배열이 비어있습니다. BP_NS_MultiPlayMode에서 폰 클래스를 할당해주세요!"));
            return;
        }

        // 사용 가능한 폰 중에서 랜덤 인덱스를 선택합니다.
        const int32 RandIndex = FMath::RandRange(0, MainGamePawnClassesToSpawn.Num() - 1);
        TSubclassOf<APawn> ChosenPawnClass = MainGamePawnClassesToSpawn[RandIndex];

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
            UE_LOG(LogTemp, Error, TEXT("No PlayerStart actors found in the level. Please place them."));
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
                UE_LOG(LogTemp, Log, TEXT("Player %s spawned and possessed random unique pawn %s."), *NewPlayer->PlayerState->GetPlayerName(), *NewPawn->GetName());

                // Flask 서버에 플레이어 로그인 알림
                NotifyPlayerLogin();
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to spawn pawn for player %s."), *NewPlayer->PlayerState->GetPlayerName());
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("PawnClass is null. Check MainGamePawnClassesToSpawn in BP."));
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

    UE_LOG(LogTemp, Warning, TEXT("[MultiPlayMode] 플레이어 로그아웃: %s"), Exiting ? *Exiting->GetName() : TEXT("Unknown"));

    // Flask 서버에 플레이어 로그아웃 알림
    NotifyPlayerLogout();

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

    UE_LOG(LogTemp, Warning, TEXT("[CheckPlayerCount] 현재 플레이어 수: %d"), ConnectedPlayerCount);

    if (ConnectedPlayerCount == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[CheckPlayerCount] 모든 플레이어가 나갔습니다. 세션을 'closed' 상태로 변경합니다."));

        if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
        {
            if (GI->MyServerPort > 0)
            {
                GI->RequestUpdateSessionStatus(GI->MyServerPort, TEXT("closed"));
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("[CheckPlayerCount] UNS_GameInstance의 MyServerPort가 유효하지 않습니다."));
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
        UE_LOG(LogTemp, Warning, TEXT("플레이어 %s는 이미 로딩 완료 목록에 있음"), *Player->GetName());
        return;
    }

    // 완료된 플레이어 목록에 추가
    LoadingCompletedPlayers.Add(Player);
    UE_LOG(LogTemp, Warning, TEXT("플레이어 %s 로딩 완료 (%d/%d)"),
        *Player->GetName(), LoadingCompletedPlayers.Num(), GetNumPlayers());

    // 모든 플레이어가 완료되었는지 확인
    CheckAllPlayersLoadingComplete();
}

void ANS_MultiPlayMode::CheckAllPlayersLoadingComplete()
{
    if (!HasAuthority()) return;

    int32 TotalPlayers = GetNumPlayers();
    int32 CompletedPlayers = LoadingCompletedPlayers.Num();

    UE_LOG(LogTemp, Warning, TEXT("로딩 완료 체크: %d/%d 플레이어"), CompletedPlayers, TotalPlayers);

    // 모든 플레이어가 로딩 완료되었으면
    if (CompletedPlayers >= TotalPlayers && TotalPlayers > 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("=== 모든 플레이어 로딩 완료! 동시에 로딩 스크린 숨기기 ==="));

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

// 좀비 스폰 체크 함수 오버라이드
void ANS_MultiPlayMode::CheckAndSpawnZombies()
{
    // 게임 오버 상태면 좀비 스폰 중단
    if (bIsGameOver)
    {
        return;
    }

    // 현재 좀비 수가 최대치에 도달했는지 확인
    int32 Missing = MaxZombieCount - CurrentZombieCount;
    if (Missing <= 0)
    {
        return;
    }

    // 살아있는 플레이어 위치 수집
    TArray<FVector> PlayerLocations;
    TArray<FString> PlayerNames; // 디버그용 플레이어 이름 저장

    if (const ANS_GameState* GS = GetGameState<ANS_GameState>())
    {
        for (APlayerState* PS : GS->PlayerArray)
        {
            if (ANS_MainGamePlayerState* MPS = Cast<ANS_MainGamePlayerState>(PS))
            {
                if (MPS->bIsAlive)
                {
                    if (APawn* AlivePawn = MPS->GetPawn())
                    {
                        PlayerLocations.Add(AlivePawn->GetActorLocation());
                        PlayerNames.Add(PS->GetPlayerName()); // 플레이어 이름 저장
                    }
                }
            }
        }
    }

    // 살아있는 플레이어가 없으면 기본 로직 사용
    if (PlayerLocations.Num() <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MultiPlayMode] 살아있는 플레이어가 없어 기본 좀비 스폰 로직 사용"));
        Super::CheckAndSpawnZombies();
        return;
    }

    // 한 번에 스폰할 좀비 수 계산 (최대 Missing까지)
    int32 SpawnCount = FMath::Min(ZombiesPerSpawn, Missing);

   /* UE_LOG(LogTemp, Warning, TEXT("[MultiPlayMode] 좀비 스폰 시작: 현재 좀비 %d/%d, 살아있는 플레이어 %d명, 스폰 예정 좀비 %d마리"),
        CurrentZombieCount, MaxZombieCount, PlayerLocations.Num(), SpawnCount);*/

    // 각 플레이어 주변에 좀비 스폰 (계산된 수만큼)
    int32 SpawnedCount = 0;
    int32 PlayerIndex = 0;

    // 모든 플레이어를 순회하면서 좀비 스폰 시도
    while (SpawnedCount < SpawnCount && PlayerIndex < PlayerLocations.Num())
    {
        const FVector& CurrentPlayerLocation = PlayerLocations[PlayerIndex];
        FString PlayerName = PlayerNames.IsValidIndex(PlayerIndex) ? PlayerNames[PlayerIndex] : FString::Printf(TEXT("Player %d"), PlayerIndex);

        // 해당 플레이어 주변의 적합한 스포너 찾기
        TArray<AANS_ZombieSpawner*> SuitableSpawners = FindSuitableSpawnersForMultiplay(CurrentPlayerLocation, PlayerLocations);

        // 적합한 스포너가 없으면 다음 플레이어로
        if (SuitableSpawners.Num() <= 0)
        {
            //UE_LOG(LogTemp, Warning, TEXT("[MultiPlayMode] %s 주변에 적합한 스포너가 없음"), *PlayerName);
            PlayerIndex = (PlayerIndex + 1) % PlayerLocations.Num(); // 다음 플레이어로 순환
            continue;
        }

        // 스포너 목록 무작위 섞기
        Algo::RandomShuffle(SuitableSpawners);

        // 해당 플레이어 주변에 좀비 스폰
        AANS_ZombieSpawner* SelectedSpawner = SuitableSpawners[0];
        FVector SpawnerLocation = SelectedSpawner->GetActorLocation();
        float DistanceToPlayer = FVector::Dist(CurrentPlayerLocation, SpawnerLocation);

       /* UE_LOG(LogTemp, Warning, TEXT("[MultiPlayMode] %s 주변 %.1f 거리에 있는 스포너에서 좀비 스폰 시도"),
            *PlayerName, DistanceToPlayer);*/

        SpawnZombieAtPoint(SelectedSpawner);
        SpawnedCount++;

       /* UE_LOG(LogTemp, Warning, TEXT("[MultiPlayMode] %s 주변에 좀비 스폰 성공 (%d/%d)"),
            *PlayerName, SpawnedCount, SpawnCount);*/

        // 다음 플레이어로 순환 (공평하게 분배)
        PlayerIndex = (PlayerIndex + 1) % PlayerLocations.Num();
    }

   /* UE_LOG(LogTemp, Warning, TEXT("[MultiPlayMode] 좀비 스폰 완료: %d마리 스폰됨, 현재 좀비 %d/%d"),
        SpawnedCount, CurrentZombieCount, MaxZombieCount);*/
}

// 플레이어로부터 너무 멀리 있는 좀비 제거 함수 (멀티플레이 버전)
void ANS_MultiPlayMode::CleanupDistantZombies()
{
    // 함수 호출 확인 로그
   // UE_LOG(LogTemp, Warning, TEXT("[MultiPlayMode] CleanupDistantZombies 함수 호출됨"));

    // 살아있는 플레이어 위치 수집
    TArray<FVector> PlayerLocations;
    if (const ANS_GameState* GS = GetGameState<ANS_GameState>())
    {
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

    //UE_LOG(LogTemp, Warning, TEXT("[MultiPlayMode] 살아있는 플레이어 수: %d"), PlayerLocations.Num());

    // 살아있는 플레이어가 없으면 기본 로직 사용
    if (PlayerLocations.Num() <= 0)
    {
       // UE_LOG(LogTemp, Warning, TEXT("[MultiPlayMode] 살아있는 플레이어가 없어 기본 로직 사용"));
        Super::CleanupDistantZombies();
        return;
    }

    // 캐시된 좀비 리스트 사용 (GetAllActorsOfClass 대신)
    TArray<ANS_ZombieBase*> AllZombies = GetCachedZombies();

   // UE_LOG(LogTemp, Warning, TEXT("[MultiPlayMode] 현재 좀비 수: %d"), AllZombies.Num());

    int32 DestroyedCount = 0;

    // 거리별 좀비 수 초기화
    ZombiesInCloseRange = 0;
    ZombiesInMidRange = 0;

    // 각 좀비의 거리 확인 및 제거 (최적화된 버전)
    float ZombieDestroyDistanceSquared = ZombieDestroyDistance * ZombieDestroyDistance;

    for (ANS_ZombieBase* Zombie : AllZombies)
    {
        if (!IsValid(Zombie))
        {
            continue;
        }

        // 모든 플레이어와의 최소 거리 계산 (DistSquared 사용)
        float MinDistanceSquared = MAX_FLT;
        FVector ZombieLocation = Zombie->GetActorLocation();

        for (const FVector& PlayerLoc : PlayerLocations)
        {
            float DistanceSquared = FVector::DistSquared(PlayerLoc, ZombieLocation);
            MinDistanceSquared = FMath::Min(MinDistanceSquared, DistanceSquared);
        }

        // 거리에 따라 카운트 (DistSquared 기준으로 변경)
        if (MinDistanceSquared <= 16000000.0f) // 4000^2
        {
            ZombiesInCloseRange++;
        }
        else if (MinDistanceSquared <= 64000000.0f) // 8000^2
        {
            ZombiesInMidRange++;
        }

        // 모든 플레이어로부터 설정된 거리보다 멀리 있으면 제거
        if (MinDistanceSquared > ZombieDestroyDistanceSquared)
        {
            // 좀비 제거 (OnZombieDestroyed 이벤트가 자동으로 호출됨)
           // UE_LOG(LogTemp, Warning, TEXT("[MultiPlayMode] 좀비 제거: 거리 %.2f"), FMath::Sqrt(MinDistanceSquared));
            Zombie->Destroy();
            DestroyedCount++;
        }
    }

    // 제거된 좀비 수 업데이트
    ZombiesRemoved += DestroyedCount;

    // 디버그 로그 출력
    UE_LOG(LogTemp, Warning, TEXT("[멀티 좀비 디버그] 총 좀비 수: %d"), AllZombies.Num());
    UE_LOG(LogTemp, Warning, TEXT("[멀티 좀비 디버그] 4000 이내 좀비: %d"), ZombiesInCloseRange);
    UE_LOG(LogTemp, Warning, TEXT("[멀티 좀비 디버그] 4000-8000 사이 좀비: %d"), ZombiesInMidRange);
    UE_LOG(LogTemp, Warning, TEXT("[멀티 좀비 디버그] 8000 초과 좀비: %d"), AllZombies.Num() - ZombiesInCloseRange - ZombiesInMidRange);

    // 제거된 좀비가 있으면 로그 출력
    if (DestroyedCount > 0)
    {
        //UE_LOG(LogTemp, Warning, TEXT("[MultiPlayMode] %d개의 멀리 있는 좀비 제거됨 (총 %d개 제거됨)"), DestroyedCount, ZombiesRemoved);
    }
    else
    {
       // UE_LOG(LogTemp, Display, TEXT("[MultiPlayMode] 제거할 좀비가 없음"));
    }
}

// 멀티플레이용 적합한 스포너 찾기 함수 (다른 플레이어 위치 고려)
TArray<AANS_ZombieSpawner*> ANS_MultiPlayMode::FindSuitableSpawnersForMultiplay(const FVector& CurrentPlayerLocation, const TArray<FVector>& AllPlayerLocations)
{
    TArray<AANS_ZombieSpawner*> SuitableSpawners;
    TMap<AANS_ZombieSpawner*, bool> SpawnerHasZombie;

    // 캐시된 좀비 위치 확인 (GetAllActorsOfClass 대신)
    TArray<ANS_ZombieBase*> ExistingZombies = GetCachedZombies();

    // 디버그 카운터 초기화
    int32 TotalSpawners = 0;
    int32 DisabledSpawners = 0;
    int32 OccupiedSpawners = 0;
    int32 OutOfRangeSpawners = 0;
    int32 TooCloseToOtherPlayerSpawners = 0;

    // 월드에 좀비가 있으면
    if (ExistingZombies.Num() > 0)
    {
        // 각 좀비의 가장 가까운 스포너 찾기 (최적화된 버전)
        TArray<AANS_ZombieSpawner*> CachedSpawnerList = GetCachedSpawners();

        for (ANS_ZombieBase* Zombie : ExistingZombies)
        {
            if (!IsValid(Zombie)) continue;

            FVector ZombieLocation = Zombie->GetActorLocation();
            float ClosestDistanceSquared = MAX_FLT;
            AANS_ZombieSpawner* ClosestSpawner = nullptr;

            // 캐시된 스포너 리스트 사용 및 DistSquared로 최적화
            for (AANS_ZombieSpawner* Spawner : CachedSpawnerList)
            {
                if (!IsValid(Spawner)) continue;

                float DistanceSquared = FVector::DistSquared(ZombieLocation, Spawner->GetActorLocation());
                if (DistanceSquared < ClosestDistanceSquared)
                {
                    ClosestDistanceSquared = DistanceSquared;
                    ClosestSpawner = Spawner;
                }
            }

            // 좀비가 스포너 근처에 있으면 해당 스포너는 사용 중으로 표시 (500^2 = 250000)
            if (ClosestSpawner && ClosestDistanceSquared < 250000.0f)
            {
                SpawnerHasZombie.Add(ClosestSpawner, true);
            }
        }
    }

    // 플레이어 인덱스 찾기 (디버그용)
    int32 CurrentPlayerIndex = -1;
    for (int32 i = 0; i < AllPlayerLocations.Num(); i++)
    {
        if (AllPlayerLocations[i] == CurrentPlayerLocation)
        {
            CurrentPlayerIndex = i;
            break;
        }
    }

    // 적합한 스포너 찾기
    for (AANS_ZombieSpawner* Spawner : ZombieSpawnPoints)
    {
        TotalSpawners++;

        // 유효하지 않거나 비활성화된 스포너는 제외
        if (!IsValid(Spawner) || !Spawner->bIsEnabled)
        {
            DisabledSpawners++;
            continue;
        }

        // 이미 좀비가 있는 스포너는 제외
        if (SpawnerHasZombie.Contains(Spawner) && SpawnerHasZombie[Spawner])
        {
            OccupiedSpawners++;
            continue;
        }

        // 현재 플레이어와의 거리 확인
        FVector SpawnerLocation = Spawner->GetActorLocation();
        float DistanceToCurrentPlayer = FVector::Dist(CurrentPlayerLocation, SpawnerLocation);

        // 최소/최대 거리 범위 내에 있는지 확인
        if (DistanceToCurrentPlayer >= MinSpawnDistance && DistanceToCurrentPlayer <= MaxSpawnDistance)
        {
            // 다른 모든 플레이어와의 거리 확인
            bool bTooCloseToOtherPlayer = false;
            int32 TooClosePlayerIndex = -1;
            float TooCloseDistance = 0.0f;

            for (int32 i = 0; i < AllPlayerLocations.Num(); i++)
            {
                const FVector& OtherPlayerLocation = AllPlayerLocations[i];

                // 현재 플레이어는 건너뛰기
                if (OtherPlayerLocation == CurrentPlayerLocation)
                {
                    continue;
                }

                float DistanceToOtherPlayer = FVector::Dist(OtherPlayerLocation, SpawnerLocation);

                // 다른 플레이어가 스포너 근처에 있으면 제외 (MinSpawnDistance 이내)
                if (DistanceToOtherPlayer < MinSpawnDistance)
                {
                    bTooCloseToOtherPlayer = true;
                    TooClosePlayerIndex = i;
                    TooCloseDistance = DistanceToOtherPlayer;
                    break;
                }
            }

            // 다른 플레이어에게 너무 가까운 스포너는 제외
            if (bTooCloseToOtherPlayer)
            {
                TooCloseToOtherPlayerSpawners++;

                // 디버그 로그 추가
                /*UE_LOG(LogTemp, Warning, TEXT("[MultiPlayMode] 플레이어 %d의 스폰 가능 구역(거리: %.1f)에 플레이어 %d가 너무 가까이 있어서(거리: %.1f < %.1f) 좀비 스폰 불가"),
                    CurrentPlayerIndex, DistanceToCurrentPlayer, TooClosePlayerIndex, TooCloseDistance, MinSpawnDistance);*/

                continue;
            }

            SuitableSpawners.Add(Spawner);
        }
        else
        {
            OutOfRangeSpawners++;
        }
    }

    // 종합 디버그 로그
   /* UE_LOG(LogTemp, Warning, TEXT("[MultiPlayMode] 플레이어 %d 스포너 검색 결과: 총 %d개 중 적합한 스포너 %d개"),
        CurrentPlayerIndex, TotalSpawners, SuitableSpawners.Num());
    UE_LOG(LogTemp, Warning, TEXT("[MultiPlayMode] 제외된 스포너: 비활성화 %d개, 사용중 %d개, 범위 밖 %d개, 다른 플레이어 근처 %d개"),
        DisabledSpawners, OccupiedSpawners, OutOfRangeSpawners, TooCloseToOtherPlayerSpawners);*/

    return SuitableSpawners;
}

// 최적화된 멀티플레이 스폰 체크
void ANS_MultiPlayMode::OptimizedMultiplaySpawnCheck()
{
    // 서버 부하 체크 - 프레임 시간이 33ms(30FPS) 이상이면 건너뜀
    if (GetWorld()->GetDeltaSeconds() > 0.033f)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MultiPlayMode] 서버 부하로 인해 스폰 체크 건너뜀"));
        return;
    }

    // 게임 오버 상태면 스폰 중단
    if (bIsGameOver)
    {
        return;
    }

    // 기존 멀티플레이 스폰 로직 실행
    CheckAndSpawnZombies();
}