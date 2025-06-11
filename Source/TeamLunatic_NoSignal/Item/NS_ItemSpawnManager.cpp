#include "Item/NS_ItemSpawnManager.h"
#include "NavigationSystem.h"
#include "World/Pickup.h"
#include "Kismet/GameplayStatics.h"
#include "Item/NS_InventoryBaseItem.h"


ANS_ItemSpawnManager::ANS_ItemSpawnManager()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ANS_ItemSpawnManager::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        //FindAndSpawnItems();
        K2_ExecuteSpawning();
    }
}

void ANS_ItemSpawnManager::SpawnItemsInRandomLocations(float Radius)
{
    UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
    if (!NavSystem)
    {
        UE_LOG(LogTemp, Error, TEXT("[%s] 네비게이션 시스템을 찾을 수 없습니다."), *GetName());
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("[%s] 스폰 시작. NumberOfItems: %d, Radius: %f"), *GetName(), SpawnItemNum, Radius);

    FNavLocation ProjectedLocation;

    for (int32 i = 0; i < SpawnItemNum; ++i)
    {
        FNavLocation RandomLocation;

        if (NavSystem->GetRandomReachablePointInRadius(ProjectedLocation.Location, Radius, RandomLocation))
        {
            // 성공
            SpawnRandomItemAt(FTransform(RandomLocation.Location));
        }
    }
}

void ANS_ItemSpawnManager::FindAndSpawnItems()
{
    if (SpawnPointTagToFind.IsNone())
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnManager '%s': 'SpawnPointTagToFind'가 설정되지 않았습니다."), *GetName());
        return;
    }

    //설정된 태그를 가진 모든 스폰 포인트를 월드에서 찾습니다
    TArray<AActor*> FoundSpawnPoints;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), SpawnPointTagToFind, FoundSpawnPoints);

    if (FoundSpawnPoints.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnManager '%s': 태그 '%s'를 가진 스폰 포인트를 찾을 수 없습니다."), *GetName(), *SpawnPointTagToFind.ToString());
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("Found %d spawn points with tag '%s'. Spawning items..."), FoundSpawnPoints.Num(), *SpawnPointTagToFind.ToString());

    //찾은 각각의 스폰 포인트를 순회하며 아이템 스폰 함수를 호출합니다.
    for (AActor* SpawnPoint : FoundSpawnPoints)
    {
        if (SpawnPoint)
        {
            SpawnRandomItemAt(SpawnPoint->GetActorTransform());
        }
    }
}

void ANS_ItemSpawnManager::SpawnRandomItemAt(const FTransform& SpawnTransform)
{
    if (!PickupClass || !ItemDataTable || SpawnableTypes.Num() == 0) return;

    //스폰할 아이템 결정 (가중치 랜덤)
    TArray<FName> AllRowNames = ItemDataTable->GetRowNames();
    TArray<FName> FilteredRowNames;
    float TotalWeight = 0.0f;

    for (const FName& RowName : AllRowNames)
    {
        FNS_ItemDataStruct* ItemData = ItemDataTable->FindRow<FNS_ItemDataStruct>(RowName, TEXT(""));
        if (ItemData && ItemData->SpawnWeight > 0.0f && SpawnableTypes.Contains(ItemData->ItemType))
        {
            FilteredRowNames.Add(RowName);
            TotalWeight += ItemData->SpawnWeight;
        }
    }

    if (TotalWeight <= 0.0f) return;

    const float RandomWeight = FMath::FRandRange(0.0f, TotalWeight);
    float CurrentWeight = 0.0f;
    FName SelectedItemID = NAME_None;

    for (const FName& RowName : FilteredRowNames)
    {
        FNS_ItemDataStruct* ItemData = ItemDataTable->FindRow<FNS_ItemDataStruct>(RowName, TEXT(""));
        if (ItemData)
        {
            CurrentWeight += ItemData->SpawnWeight;
            if (RandomWeight <= CurrentWeight)
            {
                SelectedItemID = RowName;
                break;
            }
        }
    }

    if (SelectedItemID.IsNone()) return;

    // --- 아이템 스폰 ---
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    APickup* SpawnedPickup = GetWorld()->SpawnActor<APickup>(
        PickupClass,
        SpawnTransform.GetLocation(),
        SpawnTransform.GetRotation().Rotator(),
        SpawnParams
    );

    if (SpawnedPickup)
    {
        SpawnedPickup->ItemDataTable = this->ItemDataTable;
        SpawnedPickup->DesiredItemID = SelectedItemID;
        SpawnedPickup->ItemQuantity = 1;

        SpawnedPickup->InitializePickup(UNS_InventoryBaseItem::StaticClass(), 1);
    }
}

void ANS_ItemSpawnManager::SpawnRandomTaggedLocations()
{
    //태그가 없거나, 스폰할 개수가 0 이하라면 중단
    if (SpawnPointTagToFind.IsNone() || SpawnItemNum <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[%s] SpawnTag가 없거나 NumberToSpawn이 0 이하입니다."), *GetName());
        return;
    }

    //월드에 있는 지정된 태그를 가진 모든 액터 찾기
    TArray<AActor*> FoundSpawnPoints;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), SpawnPointTagToFind, FoundSpawnPoints);

    //찾은 위치가 없으면 로그 남기고 함수 종료
    if (FoundSpawnPoints.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[%s] 태그 '%s'를 가진 스폰 포인트를 찾을 수 없습니다."), *GetName(), *SpawnPointTagToFind.ToString());
        return;
    }
}

