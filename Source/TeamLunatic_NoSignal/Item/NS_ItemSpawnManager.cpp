#include "Item/NS_ItemSpawnManager.h"
#include "NavigationSystem.h"
#include "World/Pickup.h"
#include "Kismet/GameplayStatics.h"
#include "Item/NS_InventoryBaseItem.h"
#include "Components/SphereComponent.h"


ANS_ItemSpawnManager::ANS_ItemSpawnManager()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;

    SpawnRadiusSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SpawnRadiusSphere"));
    SetRootComponent(SpawnRadiusSphere);

    // 시각화 구체의 기본 설정
    SpawnRadiusSphere->SetCollisionProfileName(TEXT("NoCollision")); // 콜리전 없음
    SpawnRadiusSphere->SetSphereRadius(3000.f); // 기본 반경 값

    // 에디터에서는 보이지만, 실제 게임에서는 보이지 않도록 설정합니다.
    SpawnRadiusSphere->bDrawOnlyIfSelected = true;
    SpawnRadiusSphere->SetHiddenInGame(true, true);
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
    if (!HasAuthority()) return;

    UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
    if (!NavSystem)
    {
        UE_LOG(LogTemp, Error, TEXT("[%s] 네비게이션 시스템을 찾을 수 없습니다."), *GetName());
        return;
    }

    FNavLocation ProjectedLocation;
    if (!NavSystem->ProjectPointToNavigation(GetActorLocation(), ProjectedLocation, FVector(100.f, 100.f, 2000.f)))
    {
        UE_LOG(LogTemp, Error, TEXT("[%s] 스포너가 네브 메시를 인식할 수 없습니다! (프로젝션 실패)"), *GetName());
        return;
    }

    for (int32 i = 0; i < SpawnItemNum; ++i)
    {
        FNavLocation RandomLocation;
        // 수정된 ProjectedLocation.Location을 탐색의 중심으로 사용합니다.
        if (NavSystem->GetRandomReachablePointInRadius(ProjectedLocation.Location, Radius, RandomLocation))
        {
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
    if (!PickupClass)
    {
        UE_LOG(LogTemp, Error, TEXT("[%s] 스폰 실패: PickupClass가 지정되지 않았습니다! 블루프린트에서 설정해주세요."), *GetName());
        return;
    }
    if (!ItemDataTable || SpawnableTypes.Num() == 0) return;

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

        if (SelectedItemID == "9mm" || SelectedItemID == "5.56mm")
        {
            SpawnedPickup->ItemQuantity = 10;
        }
        else
        {
            SpawnedPickup->ItemQuantity = 1;
        }

        SpawnedPickup->InitializePickup(UNS_InventoryBaseItem::StaticClass(), 1);
    }
}

void ANS_ItemSpawnManager::SpawnRandomTaggedLocations()
{
    if (!HasAuthority()) return;

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

    const int32 TotalFound = FoundSpawnPoints.Num();
    const int32 ActualNumToSpawn = FMath::Min(SpawnItemNum, TotalFound);

    for (int32 i = 0; i < ActualNumToSpawn; ++i)
    {
        const int32 RandomIndex = FMath::RandRange(0, FoundSpawnPoints.Num() - 1);
        AActor* ChosenSpawnPoint = FoundSpawnPoints[RandomIndex];

        if (ChosenSpawnPoint)
        {
            SpawnRandomItemAt(ChosenSpawnPoint->GetActorTransform());
        }

        FoundSpawnPoints.RemoveAt(RandomIndex);
    }

    UE_LOG(LogTemp, Log, TEXT("[%s] 태그 '%s'를 가진 %d개의 스폰 지점 중 %d개를 선택하여 스폰했습니다."), *GetName(), *SpawnPointTagToFind.ToString(), TotalFound, ActualNumToSpawn);
}

