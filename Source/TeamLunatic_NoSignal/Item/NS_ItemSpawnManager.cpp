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

void ANS_ItemSpawnManager::SpawnEndingItems()
{
    if (!HasAuthority()) return;

    // 1. 유효성 검사 (데이터 테이블, 픽업 클래스)
    if (!ItemDataTable || !PickupClass)
    {
        UE_LOG(LogTemp, Error, TEXT("[%s] SpawnEndingItems 실패: ItemDataTable 또는 PickupClass가 설정되지 않았습니다."), *GetName());
        return;
    }

    // 2. 데이터 테이블에서 EEndingType과 아이템 ID(RowName)를 미리 매핑 (이전과 동일)
    TMap<EEndingType, FName> EndingItemRowNames;
    TArray<FName> AllRowNames = ItemDataTable->GetRowNames();
    for (const FName& RowName : AllRowNames)
    {
        FNS_ItemDataStruct* ItemData = ItemDataTable->FindRow<FNS_ItemDataStruct>(RowName, TEXT(""));
        if (ItemData && ItemData->ItemType == EItemType::EndingTrigger && ItemData->EndingType != EEndingType::None)
        {
            EndingItemRowNames.Add(ItemData->EndingType, RowName);
        }
    }

    // 3. [수정된 로직] 스폰할 아이템 목록을 순회합니다.
    for (const TPair<EEndingType, int32>& Pair : EndingItemsToSpawn)
    {
        const EEndingType TypeToSpawn = Pair.Key;
        const int32 NumberToSpawn = Pair.Value;

        // 3-1. 해당 타입에 지정된 스폰 태그를 찾습니다.
        const FName* SpawnTagPtr = EndingItemSpawnTags.Find(TypeToSpawn);
        if (!SpawnTagPtr || SpawnTagPtr->IsNone())
        {
            UE_LOG(LogTemp, Warning, TEXT("EndingType %d에 대한 스폰 태그가 지정되지 않았습니다. 이 타입의 스폰을 건너뜁니다."), static_cast<int32>(TypeToSpawn));
            continue; // 다음 타입으로 넘어감
        }

        // 3-2. 찾은 태그로 월드에서 해당 타입의 스폰 위치 목록을 가져옵니다.
        TArray<AActor*> FoundSpawnPoints;
        UGameplayStatics::GetAllActorsWithTag(GetWorld(), *SpawnTagPtr, FoundSpawnPoints);

        if (FoundSpawnPoints.Num() < NumberToSpawn)
        {
            UE_LOG(LogTemp, Warning, TEXT("태그 '%s'를 가진 스폰 포인트 개수(%d)가 스폰할 아이템 개수(%d)보다 적습니다."), *SpawnTagPtr->ToString(), FoundSpawnPoints.Num(), NumberToSpawn);
        }
        if (FoundSpawnPoints.Num() == 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("태그 '%s'를 가진 스폰 포인트를 찾을 수 없습니다."), *SpawnTagPtr->ToString());
            continue; // 다음 타입으로 넘어감
        }

        // 3-3. 스폰할 아이템의 ID를 맵에서 찾습니다.
        const FName* ItemIDPtr = EndingItemRowNames.Find(TypeToSpawn);
        if (!ItemIDPtr)
        {
            UE_LOG(LogTemp, Warning, TEXT("데이터 테이블에서 EndingType %d 에 해당하는 아이템을 찾을 수 없습니다."), static_cast<int32>(TypeToSpawn));
            continue; // 다음 타입으로 넘어감
        }

        // 3-4. 해당 타입의 아이템을 지정된 개수만큼 스폰합니다.
        for (int32 i = 0; i < NumberToSpawn; ++i)
        {
            const int32 RandomIndex = FMath::RandRange(0, FoundSpawnPoints.Num() - 1);
            AActor* ChosenSpawnPoint = FoundSpawnPoints[RandomIndex];
            FoundSpawnPoints.RemoveAt(RandomIndex);

            if (ChosenSpawnPoint)
            { 
                SpawnSpecificItemAt(*ItemIDPtr, ChosenSpawnPoint->GetActorTransform());
            }
        }

    }
    UE_LOG(LogTemp, Log, TEXT("[%s] 엔딩 아이템 스폰 완료."), *GetName());
}

void ANS_ItemSpawnManager::SpawnSpecificItemAt(FName ItemID, const FTransform& SpawnTransform)
{
    if (!PickupClass || ItemID.IsNone())
    {
        UE_LOG(LogTemp, Error, TEXT("[%s] SpawnSpecificItemAt 실패: PickupClass가 없거나 ItemID가 None입니다."), *GetName());
        return;
    }

    // --- 아이템 스폰 ---
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    // 월드에 Pickup 액터를 스폰합니다.
    APickup* SpawnedPickup = GetWorld()->SpawnActor<APickup>(
        PickupClass,
        SpawnTransform.GetLocation(),
        SpawnTransform.GetRotation().Rotator(),
        SpawnParams
    );

    // 스폰이 성공적으로 되었다면
    if (SpawnedPickup)
    {
        // 스폰된 픽업에 필요한 정보를 설정합니다.
        SpawnedPickup->ItemDataTable = this->ItemDataTable;
        // 랜덤으로 결정된 ID가 아닌, 파라미터로 받은 '특정' ItemID를 사용합니다.
        SpawnedPickup->DesiredItemID = ItemID;
        // 엔딩 아이템의 수량은 1로 고정합니다. 필요 시 데이터 테이블에서 읽어오도록 수정할 수 있습니다.
        SpawnedPickup->ItemQuantity = 1;

        SpawnedPickup->InitializePickup(UNS_InventoryBaseItem::StaticClass(), 1);

        UE_LOG(LogTemp, Log, TEXT("아이템 '%s'를 위치 %s에 스폰했습니다."), *ItemID.ToString(), *SpawnTransform.GetLocation().ToString());
    }
}

