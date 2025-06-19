// Fill out your copyright notice in the Description page of Project Settings.


#include "NS_EndingTriggerZone.h"
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

ANS_EndingTriggerZone::ANS_EndingTriggerZone()
{
    PrimaryActorTick.bCanEverTick = false;

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    RootComponent = TriggerBox;
    TriggerBox->SetBoxExtent(FVector(200.f));
    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    TriggerBox->SetGenerateOverlapEvents(true);

    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ANS_EndingTriggerZone::OnOverlapBegin);
    UE_LOG(LogTemp, Warning, TEXT("TriggerBox OnOverlapBegin event bound"));
    TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ANS_EndingTriggerZone::OnOverlapEnd);
    UE_LOG(LogTemp, Warning, TEXT("TriggerBox OnOverlapEnd event bound"));

    EndingStatusWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("EndingStatusWidget"));
    EndingStatusWidget->SetupAttachment(RootComponent);
    EndingStatusWidget->SetWidgetSpace(EWidgetSpace::World);
    EndingStatusWidget->SetVisibility(false); 

    bReplicates = true;
}

void ANS_EndingTriggerZone::BeginPlay()
{
    Super::BeginPlay();
}

void ANS_EndingTriggerZone::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (ANS_PlayerCharacterBase* Player = Cast<ANS_PlayerCharacterBase>(OtherActor))
    {
        OverlappingPlayers.AddUnique(Player);
        CheckGroupEndingCondition();
    }
}

void ANS_EndingTriggerZone::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (ANS_PlayerCharacterBase* Player = Cast<ANS_PlayerCharacterBase>(OtherActor))
    {
        OverlappingPlayers.Remove(Player);
        CheckGroupEndingCondition();
    }
}

void ANS_EndingTriggerZone::CheckGroupEndingCondition()
{
    int32 NumPlayersInZone = 0;
    bool bHasBattery = false;
    bool bHasWalkietalkie = false;

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
            if (!bHasBattery && RowName == "Battery")
            {
                bHasBattery = true;
                UE_LOG(LogTemp, Warning, TEXT("아이템: %s"), *Item->ItemDataRowName.ToString());
            }
            else if (!bHasWalkietalkie && RowName == "Walkietalkie")
            {
                bHasWalkietalkie = true;
                UE_LOG(LogTemp, Warning, TEXT("아이템: %s"), *Item->ItemDataRowName.ToString());
            }

            if (bHasBattery && bHasWalkietalkie)
            {
                break; 
            }
        }
    }

    int32 NumCollectedItems = 0;
    if (bHasBattery) NumCollectedItems++;
    if (bHasWalkietalkie) NumCollectedItems++;

    UpdateWidgetStatus(NumPlayersInZone, NumCollectedItems);

    const bool bGroupConditionMet = bHasBattery && bHasWalkietalkie;

    if (bGroupConditionMet)
    {
        // 조건 처음 충족되었을 때 타이머 시작
        if (!bIsEndingTimerRunning)
        {
            UE_LOG(LogTemp, Warning, TEXT("[엔딩] 조건 충족 → 10초 타이머 시작"));

            GetWorld()->GetTimerManager().SetTimer(EndingConditionTimerHandle, this, &ANS_EndingTriggerZone::EndingConditionSatisfied, 10.0f, false);
            // 타이머 텍스트 업데이트용 반복 타이머 추가
            GetWorld()->GetTimerManager().SetTimer(CountdownUpdateTimerHandle, this, &ANS_EndingTriggerZone::UpdateEndingCountdownUI, 0.1f, true);

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

void ANS_EndingTriggerZone::UpdateWidgetStatus(int32 NumPlayers, int32 NumItems)
{
    if (!EndingStatusWidget) return;

    UUserWidget* Widget = EndingStatusWidget->GetUserWidgetObject();
    if (!Widget) return;

    UNS_EndingStatusUI* StatusUI = Cast<UNS_EndingStatusUI>(Widget);
    if (StatusUI)
    {
        StatusUI->EndingUpdateStatus(NumPlayers, NumItems);
    }

    // 트리거 안에 아무도 없으면 숨김
    EndingStatusWidget->SetVisibility(NumPlayers > 0);
}

void ANS_EndingTriggerZone::UpdateEndingCountdownUI()
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

void ANS_EndingTriggerZone::EndingConditionSatisfied()
{
    UE_LOG(LogTemp, Warning, TEXT("[엔딩] 조건 유지 10초 완료 → 엔딩 처리"));

    Multicast_TriggerEnding(true);
    bIsEndingTimerRunning = false;
}

void ANS_EndingTriggerZone::Multicast_TriggerEnding_Implementation(bool bGroupConditionMet)
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
        Multicast_ShowEndingResultList(SuccessNames, FailNames);

        UE_LOG(LogTemp, Warning, TEXT("Player %s: %s"),
            *Player->GetName(), bSuccess ? TEXT("탈출 성공") : TEXT("탈출 실패"));
    }
}

void ANS_EndingTriggerZone::Multicast_ShowEndingResultList_Implementation(const TArray<FString>& SuccessList, const TArray<FString>& FailList)
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
        EndingWidget->SetEndingType(FName("Radio"));

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
