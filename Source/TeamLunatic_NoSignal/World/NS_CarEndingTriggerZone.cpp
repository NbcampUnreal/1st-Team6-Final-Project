// Fill out your copyright notice in the Description page of Project Settings.


#include "World/NS_CarEndingTriggerZone.h"
#include "Components/BoxComponent.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Inventory/InventoryComponent.h"
#include "Item/NS_InventoryBaseItem.h"

#include "World/EndingUI/NS_EndingStatusUI.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"
#include "World/EndingUI/EndingResultWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include <Kismet/GameplayStatics.h>


ANS_CarEndingTriggerZone::ANS_CarEndingTriggerZone()
{
    PrimaryActorTick.bCanEverTick = true;

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    RootComponent = TriggerBox;
    TriggerBox->SetBoxExtent(FVector(200.f));
    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    TriggerBox->SetGenerateOverlapEvents(true);

    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ANS_CarEndingTriggerZone::OnOverlapBegin);
    UE_LOG(LogTemp, Warning, TEXT("TriggerBox OnOverlapBegin event bound"));
    TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ANS_CarEndingTriggerZone::OnOverlapEnd);
    UE_LOG(LogTemp, Warning, TEXT("TriggerBox OnOverlapEnd event bound"));

    EndingStatusWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("EndingStatusWidget"));
    EndingStatusWidget->SetupAttachment(RootComponent);
    EndingStatusWidget->SetWidgetSpace(EWidgetSpace::World);
    EndingStatusWidget->SetVisibility(true);
    EndingStatusWidget->SetDrawAtDesiredSize(true);     // 위젯 크기 자동
    EndingStatusWidget->SetPivot(FVector2D(0.5f, 0.5f)); // 중심 기준
    EndingStatusWidget->SetTwoSided(true);              // 후면에서도 보이게

    bReplicates = true;
}

void ANS_CarEndingTriggerZone::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 서버에서만 플레이어 수 추적
    if (HasAuthority())
    {
        int32 CurrentPlayerCount = 0;

        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            APlayerController* PC = It->Get();
            if (PC && PC->GetPawn())  // Pawn 있는 경우만 카운트
            {
                ++CurrentPlayerCount;
            }
        }

        // 변화가 있을 때만 갱신
        if (CachedTotalPlayerCount != CurrentPlayerCount)
        {
            CachedTotalPlayerCount = CurrentPlayerCount;
            UE_LOG(LogTemp, Warning, TEXT("실시간 전체 플레이어 수 변경 감지: %d"), CachedTotalPlayerCount);

            // UI 갱신
            int32 NumPlayersInZone = 0;
            for (AActor* Player : OverlappingPlayers)
            {
                if (IsValid(Player))
                {
                    NumPlayersInZone++;
                }
            }

            UpdateWidgetStatus(NumPlayersInZone, CachedTotalPlayerCount, 0);
        }
    }

    // 서버에선 회전 처리 안 함
    if (GetNetMode() == NM_DedicatedServer) return;

    // 위젯 유효성 확인
    if (!EndingStatusWidget) return;

    // 시간 간격 체크
    TimeSinceLastUpdate += DeltaTime;
    if (TimeSinceLastUpdate < RotationUpdateInterval) return;
    TimeSinceLastUpdate = 0.f;

    // 카메라 위치 획득
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!IsValid(PC) || !PC->IsLocalController()) return;

    APlayerCameraManager* CamManager = PC->PlayerCameraManager;
    if (!CamManager) return;

    FVector CamLocation = CamManager->GetCameraLocation();
    FVector WidgetLocation = EndingStatusWidget->GetComponentLocation();

    float DistanceSq = FVector::DistSquared(CamLocation, WidgetLocation);
    const bool bShouldBeVisible = DistanceSq <= FMath::Square(MaxVisibleDistance);

    // 바뀐 경우에만 SetVisibility 호출
    if (bShouldBeVisible != bPreviouslyVisible)
    {
        EndingStatusWidget->SetVisibility(bShouldBeVisible);
        bPreviouslyVisible = bShouldBeVisible;

        UE_LOG(LogTemp, Verbose, TEXT("EndingWidget %s"),
            bShouldBeVisible ? TEXT("표시됨") : TEXT("숨김"));
    }

    // 회전은 UI가 보일 때만 적용
    if (bShouldBeVisible)
    {
        // LookAt 방향 계산
        FVector DirectionToCamera = CamLocation - WidgetLocation;

        // Pitch, Roll 제거 → Yaw만 유지
        FRotator LookAtRotation = FRotationMatrix::MakeFromX(DirectionToCamera).Rotator();
        FRotator OnlyYawRotation = FRotator(0.f, LookAtRotation.Yaw, 0.f);

        // 현재 회전과 큰 차이가 있을 때만 적용 (불필요한 호출 방지)
        FRotator CurrentRotation = EndingStatusWidget->GetComponentRotation();
        if (!CurrentRotation.Equals(OnlyYawRotation, 1.0f)) // 오차 허용값 1도
        {
            EndingStatusWidget->SetWorldRotation(OnlyYawRotation);
        }
    }
}


// Called when the game starts or when spawned
void ANS_CarEndingTriggerZone::BeginPlay()
{
	Super::BeginPlay();
}

void ANS_CarEndingTriggerZone::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (ANS_PlayerCharacterBase* Player = Cast<ANS_PlayerCharacterBase>(OtherActor))
    {
        OverlappingPlayers.AddUnique(Player);
        CheckGroupEndingCondition();
    }
}

void ANS_CarEndingTriggerZone::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (ANS_PlayerCharacterBase* Player = Cast<ANS_PlayerCharacterBase>(OtherActor))
    {
        OverlappingPlayers.Remove(Player);
        CheckGroupEndingCondition();
    }
}

void ANS_CarEndingTriggerZone::CheckGroupEndingCondition()
{
    // 전체 참여 플레이어 수 계산
    int32 TotalPlayerCount = CachedTotalPlayerCount;
    int32 NumPlayersInZone = 0;
    bool bHasPetrolTank = false;
    bool bHasCarKey = false;


    for (ANS_PlayerCharacterBase* Player : OverlappingPlayers)
    {
        UE_LOG(LogTemp, Warning, TEXT("조건 검사 시작: 중첩된 플레이어 수 = %d"), OverlappingPlayers.Num());
        if (!IsValid(Player)) continue;

        NumPlayersInZone++;

        UInventoryComponent* Inventory = Player->FindComponentByClass<UInventoryComponent>();
        if (!Inventory) continue;

        // 인벤토리 내 아이템들을 순회
        for (UNS_InventoryBaseItem* Item : Inventory->GetInventoryContents())
        {
            if (!Item) continue;

            const FName RowName = Item->ItemDataRowName;
            if (!bHasPetrolTank && RowName == "PetrolTank")
            {
                bHasPetrolTank = true;
                UE_LOG(LogTemp, Warning, TEXT("아이템: %s"), *Item->ItemDataRowName.ToString());
            }
            else if (!bHasCarKey && RowName == "CarKey")
            {
                bHasCarKey = true;
                UE_LOG(LogTemp, Warning, TEXT("아이템: %s"), *Item->ItemDataRowName.ToString());
            }

            if (bHasPetrolTank && bHasCarKey)
            {
                break;
            }
        }
    }

    int32 NumCollectedItems = 0;
    if (bHasPetrolTank) NumCollectedItems++;
    if (bHasCarKey) NumCollectedItems++;

    UpdateWidgetStatus(NumPlayersInZone, TotalPlayerCount, NumCollectedItems);

    const bool bGroupConditionMet = bHasPetrolTank && bHasCarKey;

    if (bGroupConditionMet)
    {
        // 조건 처음 충족되었을 때 타이머 시작
        if (!bIsEndingTimerRunning)
        {
            UE_LOG(LogTemp, Warning, TEXT("[엔딩] 조건 충족 → 10초 타이머 시작"));

            GetWorld()->GetTimerManager().SetTimer(EndingConditionTimerHandle, this, &ANS_CarEndingTriggerZone::EndingConditionSatisfied, 10.0f, false);
            // 타이머 텍스트 업데이트용 반복 타이머 추가
            GetWorld()->GetTimerManager().SetTimer(CountdownUpdateTimerHandle, this, &ANS_CarEndingTriggerZone::UpdateEndingCountdownUI, 0.1f, true);

            EndingCountdown = 10.0f;
            bIsEndingTimerRunning = true;

        }
    }
    else
    {
        // 조건 도중에 깨졌을 경우 타이머 중단
        if (bIsEndingTimerRunning)
        {
            UE_LOG(LogTemp, Warning, TEXT("[엔딩] 조건 불충족 → 타이머 취소"));

            GetWorld()->GetTimerManager().ClearTimer(EndingConditionTimerHandle);
            GetWorld()->GetTimerManager().ClearTimer(CountdownUpdateTimerHandle);
            bIsEndingTimerRunning = false;

            if (UNS_EndingStatusUI* StatusUI = Cast<UNS_EndingStatusUI>(EndingStatusWidget->GetUserWidgetObject()))
            {
                StatusUI->UpdateRemainingTime(0.f); // 즉시 텍스트 숨기기
            }
        }
    }
}

void ANS_CarEndingTriggerZone::UpdateWidgetStatus(int32 NumPlayers, int32 TotalPlayers, int32 NumItems)
{
    if (!EndingStatusWidget) return;

    UUserWidget* Widget = EndingStatusWidget->GetUserWidgetObject();
    if (!Widget) return;

    UNS_EndingStatusUI* StatusUI = Cast<UNS_EndingStatusUI>(Widget);
    if (StatusUI)
    {
        StatusUI->EndingUpdateStatus(NumPlayers, TotalPlayers, NumItems);
    }

    // 트리거 안에 아무도 없으면 숨김
    EndingStatusWidget->SetVisibility(NumPlayers > 0);
}

void ANS_CarEndingTriggerZone::UpdateEndingCountdownUI()
{
    EndingCountdown = FMath::Max(EndingCountdown - 0.1f, 0.f);

    // 위젯이 유효하면 시간 업데이트
    if (UNS_EndingStatusUI* StatusUI = Cast<UNS_EndingStatusUI>(EndingStatusWidget->GetUserWidgetObject()))
    {
        StatusUI->UpdateRemainingTime(EndingCountdown);
    }

    if (EndingCountdown <= 0.f)
    {
        GetWorld()->GetTimerManager().ClearTimer(CountdownUpdateTimerHandle);

        bIsEndingTimerRunning = false;
    }
}

void ANS_CarEndingTriggerZone::EndingConditionSatisfied()
{
    UE_LOG(LogTemp, Warning, TEXT("[엔딩] 조건 유지 10초 완료 → 엔딩 처리"));

    Multicast_TriggerEnding(true);
    bIsEndingTimerRunning = false;
}

void ANS_CarEndingTriggerZone::Multicast_ShowEndingResultList_Implementation(const TArray<FString>& SuccessList, const TArray<FString>& FailList)
{
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (!IsValid(PlayerController) || !PlayerController->IsLocalController()) return;

    TArray<UUserWidget*> FoundWidgets;
    UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, UEndingResultWidget::StaticClass(), false);

    for (UUserWidget* Widget : FoundWidgets)
    {
        if (UEndingResultWidget* Existing = Cast<UEndingResultWidget>(Widget))
        {
            if (Existing->IsInViewport())
            {
                // 이미 UI가 떠있다면 → 내용만 갱신
                Existing->SetPlayerResultLists(SuccessList, FailList);
                return;
            }
        }
    }

    UEndingResultWidget* EndingWidget = CreateWidget<UEndingResultWidget>(PlayerController, EndingResultWidget);
    if (IsValid(EndingWidget))
    {
        EndingWidget->AddToViewport();
        EndingWidget->SetPlayerResultLists(SuccessList, FailList);
        EndingWidget->SetEndingType(FName("Car"));

        // 게임 일시 정지
        UGameplayStatics::SetGamePaused(PlayerController->GetWorld(), true);

        // UI 입력 모드 설정
        FInputModeGameAndUI InputMode;
        InputMode.SetWidgetToFocus(EndingWidget->TakeWidget());
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

        PlayerController->SetInputMode(InputMode);
        PlayerController->bShowMouseCursor = true;
    }
}

void ANS_CarEndingTriggerZone::Multicast_TriggerEnding_Implementation(bool bGroupConditionMet)
{
    TArray<FString> SuccessNames;
    TArray<FString> FailNames;
    // 전체 플레이어 가져오기 (월드에서)
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PC = It->Get();
        ANS_PlayerCharacterBase* Player = Cast<ANS_PlayerCharacterBase>(PC ? PC->GetPawn() : nullptr);

        if (!IsValid(Player)) continue;

        // 트리거존 안에 있는지 판단
        const bool bInZone = OverlappingPlayers.Contains(Player);
        const bool bSuccess = bInZone && bGroupConditionMet;

        const FString PlayerName = Player->GetPlayerState()->GetPlayerName();

        if (bSuccess)
        {
            if (!SuccessNames.Contains(PlayerName))
            {
                SuccessNames.Add(PlayerName);
            }
        }
        else
        {
            if (!FailNames.Contains(PlayerName))
            {
                FailNames.Add(PlayerName);
            }
        }
        UE_LOG(LogTemp, Warning, TEXT("Player %s: %s"),
            *Player->GetName(), bSuccess ? TEXT("탈출 성공") : TEXT("탈출 실패"));
    }
    Multicast_ShowEndingResultList(SuccessNames, FailNames);
}

void ANS_CarEndingTriggerZone::OnRep_CachedTotalPlayerCount()
{
    UE_LOG(LogTemp, Warning, TEXT("클라이언트에서 CachedTotalPlayerCount 복제됨: %d"), CachedTotalPlayerCount);

    // 트리거 안에 중첩된 플레이어 수 다시 계산
    int32 NumPlayersInZone = 0;
    for (AActor* Player : OverlappingPlayers)
    {
        if (IsValid(Player))
        {
            NumPlayersInZone++;
        }
    }

    UpdateWidgetStatus(NumPlayersInZone, CachedTotalPlayerCount, 0);
}

void ANS_CarEndingTriggerZone::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ANS_CarEndingTriggerZone, CachedTotalPlayerCount);
}


