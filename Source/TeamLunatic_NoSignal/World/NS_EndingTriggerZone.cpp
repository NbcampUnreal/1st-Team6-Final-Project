// Fill out your copyright notice in the Description page of Project Settings.


#include "NS_EndingTriggerZone.h"
#include "Components/BoxComponent.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Inventory/InventoryComponent.h"
#include "Item/NS_InventoryBaseItem.h"
#include "Net/UnrealNetwork.h"

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
    }
}

void ANS_EndingTriggerZone::CheckGroupEndingCondition()
{
    bool bHasBattery = false;
    bool bHasAntena = false;

    for (ANS_PlayerCharacterBase* Player : OverlappingPlayers)
    {
        UE_LOG(LogTemp, Warning, TEXT("조건 검사 시작: 중첩된 플레이어 수 = %d"), OverlappingPlayers.Num());
        if (!IsValid(Player)) continue;
       
        UInventoryComponent* Inventory = Player->FindComponentByClass<UInventoryComponent>();
        if (!Inventory) continue;

        // 인벤토리 내 아이템들을 순회
        for (UNS_InventoryBaseItem* Item : Inventory->GetInventoryContents())
        {
            if (!Item) continue;

            const FName RowName = Item->ItemDataRowName;
            if (RowName == "Battery")
            {
                bHasBattery = true;
                UE_LOG(LogTemp, Warning, TEXT("아이템: %s"), *Item->ItemDataRowName.ToString());
            }
            else if (RowName == "Antena")
            {
                bHasAntena = true;
                UE_LOG(LogTemp, Warning, TEXT("아이템: %s"), *Item->ItemDataRowName.ToString());
            }
        }
    }

    if (bHasBattery && bHasAntena)
    {
        Multicast_TriggerEnding();
    }
}

void ANS_EndingTriggerZone::Multicast_TriggerEnding_Implementation()
{
    for (ANS_PlayerCharacterBase* Player : OverlappingPlayers)
    {
        if (IsValid(Player))
        {
            //Player->Client_ShowEndingUI(); // 엔딩 UI 호출 함수는 클라이언트 전용으로 정의해야 함
			UE_LOG(LogTemp, Warning, TEXT("Ending Triggered for Player: %s"), *Player->GetName());
        }
    }
}
