#include "World/Pickup.h"
#include "Engine/DataTable.h"
#include "Item/NS_ItemDataStruct.h"
#include "Item/NS_BaseWeapon.h"
#include "GameFlow/NS_GameState.h"
#include "Item/NS_InventoryBaseItem.h"
#include "Inventory/InventoryComponent.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Item/NS_BaseMagazine.h"
#include "Inventory/QSlotCom/NS_QuickSlotComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFlow/NS_GameInstance.h"
#include "UI/NS_UIManager.h"
#include "Character/NS_PlayerController.h"
#include "UI/NS_PlayerHUD.h"

APickup::APickup()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicatingMovement(true);

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>("PickupMesh");
	PickupMesh->SetSimulatePhysics(false);
	SetRootComponent(PickupMesh);
}

void APickup::BeginPlay()
{
	Super::BeginPlay();

	FString LogMessage = FString::Printf(
		TEXT("Pickup BeginPlay -> 이름: [ %s ], 월드 위치: [ %s ]"),
		*GetName(), 
		*GetActorLocation().ToString() 
	);

	UE_LOG(LogTemp, Log, TEXT("%s"), *LogMessage);



	if (HasAuthority() && DesiredItemID == FName("Memo"))
	{
		if (UNS_GameInstance* GI = GetGameInstance<UNS_GameInstance>())
		{
			if (UNS_UIManager* UIManager = GI->GetUIManager())
			{
				UIManager->OnPlayerHUDReady.AddDynamic(this, &APickup::OnHUDReady);
				UE_LOG(LogTemp, Log, TEXT("Pickup '%s'이 HUD 준비 완료 이벤트를 구독합니다."), *GetName());
			}
		}
	}

	if (HasAuthority())
	{
		InitializePickup(UNS_InventoryBaseItem::StaticClass(), ItemQuantity);

		if (DesiredItemID == FName("Memo"))
		{
			FTimerHandle DelayHandle;
			GetWorldTimerManager().SetTimer(
				DelayHandle,
				this,
				&APickup::TryAssignToHUD,
				0.2f,
				false
			);
		}
	}
	else
	{
		OnRep_ReplicatedItemData();
	}
}

void APickup::InitializePickup(const TSubclassOf<UNS_InventoryBaseItem> BaseClass, const int32 InQuantity)
{
	if (ItemDataTable && !DesiredItemID.IsNone())
	{
		const FNS_ItemDataStruct* ItemData = ItemDataTable->FindRow<FNS_ItemDataStruct>(DesiredItemID, DesiredItemID.ToString());

		UE_LOG(LogTemp, Warning, TEXT("[PickupInit] 아이템: %s | AmmoType: %d"),
			*ItemData->ItemDataRowName.ToString(),
			static_cast<uint8>(ItemData->WeaponData.AmmoType));

		ReplicatedItemData = *ItemData;
		ReplicatedItemData.Quantity = InQuantity > 0 ? InQuantity : 1;
		OnRep_ReplicatedItemData();

		ItemReference = NewObject<UNS_InventoryBaseItem>(GetWorld(), BaseClass);
		ItemReference->ItemType = ItemData->ItemType;
		ItemReference->WeaponType = ItemData->WeaponType;
		ItemReference->WeaponData = ItemData->WeaponData;
		ItemReference->TextData = ItemData->ItemTextData;
		ItemReference->NumericData = ItemData->ItemNumericData;
		ItemReference->AssetData = ItemData->ItemAssetData;
		ItemReference->ItemDataRowName = ItemData->ItemDataRowName;
		ItemReference->CurrentAmmo = ItemData->CurrentAmmo;

		InQuantity <= 0 ? ItemReference->SetQuantity(1) : ItemReference->SetQuantity(InQuantity);

		PickupMesh->SetStaticMesh(ItemData->ItemAssetData.StaticMesh);

		UpdateInteractableData();
	}
}

void APickup::InitializeDrop(UNS_InventoryBaseItem* ItemToDrop, const int32 InQuantity)
{
	ItemReference = ItemToDrop;
	InQuantity <= 0 ? ItemReference->SetQuantity(1) : ItemReference->SetQuantity(InQuantity);
	ItemReference->NumericData.Weight = ItemToDrop->GetItemSingleWeight();
	PickupMesh->SetStaticMesh(ItemToDrop->AssetData.StaticMesh);

	ReplicatedItemData.ItemDataRowName = ItemToDrop->ItemDataRowName;
	ReplicatedItemData.ItemTextData = ItemToDrop->TextData;
	ReplicatedItemData.ItemNumericData = ItemToDrop->NumericData;
	ReplicatedItemData.ItemAssetData = ItemToDrop->AssetData;
	ReplicatedItemData.WeaponData = ItemToDrop->WeaponData;
	ReplicatedItemData.WeaponType = ItemToDrop->WeaponType;
	ReplicatedItemData.ItemType = ItemToDrop->ItemType;
	ReplicatedItemData.CurrentAmmo = ItemToDrop->CurrentAmmo;

	UpdateInteractableData();
}

void APickup::OnRep_ReplicatedItemData()
{
	if (!ItemReference)
	{
		ItemReference = NewObject<UNS_InventoryBaseItem>(GetWorld(), UNS_InventoryBaseItem::StaticClass());
	}

	ItemReference->ItemDataRowName = ReplicatedItemData.ItemDataRowName;
	ItemReference->ItemsDataTable = ItemDataTable;
	ItemReference->TextData = ReplicatedItemData.ItemTextData;
	ItemReference->NumericData = ReplicatedItemData.ItemNumericData;
	ItemReference->AssetData = ReplicatedItemData.ItemAssetData;
	ItemReference->WeaponData = ReplicatedItemData.WeaponData;
	ItemReference->WeaponType = ReplicatedItemData.WeaponType;
	ItemReference->ItemType = ReplicatedItemData.ItemType;
	ItemReference->CurrentAmmo = ReplicatedItemData.CurrentAmmo;

	PickupMesh->SetStaticMesh(ReplicatedItemData.ItemAssetData.StaticMesh);
	ItemReference->SetQuantity(ReplicatedItemData.Quantity);

	UpdateInteractableData();
}

void APickup::UpdateInteractableData()
{
	InstanceInteractableData.InteractableType = EInteractableType::Pickup;
	InstanceInteractableData.Action = ItemReference->TextData.InteractionText;
	InstanceInteractableData.Name = ItemReference->TextData.ItemName;
	InstanceInteractableData.Quantity = ItemReference->Quantity;
	InteractableData = InstanceInteractableData;
}

void APickup::BeginFocus()
{
	if (PickupMesh)
	{
		PickupMesh->SetRenderCustomDepth(true);
	}
}

void APickup::EndFocus()
{
	if (PickupMesh)
	{
		PickupMesh->SetRenderCustomDepth(false);
	}
}

void APickup::Interact_Implementation(AActor* InteractingActor)
{
	if (!HasAuthority())
	{
		Server_TakePickup(InteractingActor);
		return;
	}

	if (ANS_PlayerCharacterBase* PlayerCharacter = Cast<ANS_PlayerCharacterBase>(InteractingActor))
	{
		TakePickup(PlayerCharacter);
	}
}

void APickup::TakePickup(ANS_PlayerCharacterBase* Taker)
{
	if (!HasAuthority()) return; 

	if (!IsPendingKillPending())
	{
		if (!ItemReference || !IsValid(ItemReference))
		{
			return;
		}
		if (UInventoryComponent* PlayerInventory = Taker->GetInventory())
		{
			// 인벤토리에 아이템 추가 시도
			const FItemAddResult AddResult = PlayerInventory->HandleAddItem(ItemReference);

			// 아이템이 성공적으로 (전부 또는 부분적으로) 추가되었다면
			if (AddResult.OperationResult == EItemAddResult::TAR_AllItemAdded || AddResult.OperationResult == EItemAddResult::TAR_PartialAmountItemAdded)
			{
				// "Memo" 아이템을 주웠을 경우의 처리
				if (ItemReference->ItemDataRowName == FName("Memo"))
				{
					// 월드에 있는 모든 퀘스트 쪽지를 찾기.
					TArray<AActor*> FoundPickups;
					UGameplayStatics::GetAllActorsOfClass(GetWorld(), APickup::StaticClass(), FoundPickups);

					TArray<APickup*> QuestPickupsToShow;
					const TArray<FName> TargetNoteIDs = { FName("One"), FName("Two"), FName("Three"), FName("Four"), FName("Five") };

					for (AActor* PickupActor : FoundPickups)
					{
						APickup* QuestPickup = Cast<APickup>(PickupActor);
						if (QuestPickup && QuestPickup->GetItemData() && TargetNoteIDs.Contains(QuestPickup->GetItemData()->ItemDataRowName))
						{
							QuestPickupsToShow.Add(QuestPickup);
						}
					}

					// 모든 클라이언트에게 보내 마커를 추가하도록 명령
					if (QuestPickupsToShow.Num() > 0)
					{
						Multicast_AddMarkerToAll(QuestPickupsToShow);
					}

					// 팁 메시지를 수정
					if (ANS_GameState* GS = GetWorld()->GetGameState<ANS_GameState>())
					{
						const FText TipMessage = FText::FromString(TEXT("메모를 읽고 단서를 찾아라."));
						GS->Multicast_UpdateAllTipTexts(TipMessage);
					}
				}

				// 모든 플레이어의 HUD에서 제거하라고 명령
				Multicast_RemoveMarkerFromAll();

				// 아이템 추가 성공 시 장비 아이템인 경우 자동 퀵슬롯 할당 처리
				if (AddResult.ActualAmountAdded > 0 &&
					ItemReference->ItemType == EItemType::Equipment &&
					ItemReference->WeaponType != EWeaponType::Ammo)
				{
					// 퀵슬롯 컴포넌트 확인
					if (UNS_QuickSlotComponent* QuickSlotComp = Taker->GetQuickSlotComponent())
					{
						// 인벤토리에 추가된 아이템 찾기
						UNS_InventoryBaseItem* AddedItem = nullptr;

						// 인벤토리에서 같은 ItemDataRowName을 가진 아이템 찾기
						for (UNS_InventoryBaseItem* Item : PlayerInventory->GetInventoryContents())
						{
							if (Item && Item->ItemDataRowName == ItemReference->ItemDataRowName)
							{
								AddedItem = Item;
								break;
							}
						}

						if (AddedItem)
						{
							// 현재 선택된 퀵슬롯 인덱스 가져오기
							int32 CurrentSlotIndex = Taker->GetCurrentQuickSlotIndex();
							bool bAssignedToCurrentSlot = false;

							// 아이템이 이미 퀵슬롯에 할당되어 있는지 확인
							if (!QuickSlotComp->IsItemAlreadyAssigned(AddedItem))
							{
								int32 AssignedSlot = -1;

								// 현재 선택된 슬롯이 비어있으면 해당 슬롯에 할당
								if (!QuickSlotComp->GetItemInSlot(CurrentSlotIndex))
								{
									QuickSlotComp->AssignToSlot(CurrentSlotIndex, AddedItem);
									AssignedSlot = CurrentSlotIndex;
									bAssignedToCurrentSlot = true;
								}
								else
								{
									// 현재 선택된 슬롯이 이미 차있으면 첫 번째 빈 슬롯에 할당
									for (int32 i = 0; i < QuickSlotComp->GetMaxSlots(); i++)
									{
										if (!QuickSlotComp->GetItemInSlot(i))
										{
											QuickSlotComp->AssignToSlot(i, AddedItem);
											AssignedSlot = i;
											UE_LOG(LogTemp, Warning, TEXT("TakePickup: 빈 퀵슬롯 %d번에 아이템 할당: %s"),
												AssignedSlot + 1, *AddedItem->GetName());
											break;
										}
									}

									// 모든 슬롯이 차있는 경우 첫 번째 슬롯에 강제 할당
									if (AssignedSlot == -1)
									{
										QuickSlotComp->AssignToSlot(0, AddedItem);
										AssignedSlot = 0;
									}
								}

								// 첫 번째 아이템을 획득한 경우에만 현재 퀵슬롯 인덱스 설정
								if (QuickSlotComp->GetTotalAssignedItems() == 1)
								{
									Taker->CurrentQuickSlotIndex = AssignedSlot;
								}
							}
						}
					}
				}

				// 아이템 획득 애니메이션 시작
				Taker->IsPickUp = true;
			}

			// 전부 추가 성공 시에만 액터 파괴
			if (AddResult.OperationResult == EItemAddResult::TAR_AllItemAdded)
			{
				Destroy();
			}
		}
	}
}

void APickup::Server_TakePickup_Implementation(AActor* InteractingActor)
{
	if (ANS_PlayerCharacterBase* PlayerCharacter = Cast<ANS_PlayerCharacterBase>(InteractingActor))
	{
		TakePickup(PlayerCharacter);
	}
}

#if WITH_EDITOR
void APickup::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName ChangedPropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(APickup, DesiredItemID))
	{
		if (ItemDataTable)
		{
			if (const FNS_ItemDataStruct* ItemData = ItemDataTable->FindRow<FNS_ItemDataStruct>(DesiredItemID, DesiredItemID.ToString()))
			{
				PickupMesh->SetStaticMesh(ItemData->ItemAssetData.StaticMesh);
			}
		}
	}
}

#endif

void APickup::TryAssignToHUD()
{
	if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
	{
		if (UNS_UIManager* UIManager = GI->GetUIManager())
		{
			if (UNS_PlayerHUD* PlayerHUD = UIManager->GetPlayerHUDWidget())
			{
				PlayerHUD->SetYeddaItem(this);

				return;
			}
		}
	}

	AssignToHUDRetryCount++;

	if (AssignToHUDRetryCount < 20)
	{
		FTimerHandle RetryHandle;
		GetWorldTimerManager().SetTimer(
			RetryHandle,
			this,
			&APickup::TryAssignToHUD,
			0.2f,
			false
		);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Pickup '%s': HUD 할당을 위한 재시도 횟수를 초과했습니다."), *GetName());
	}
}
void APickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APickup, ReplicatedItemData);
}

void APickup::Multicast_AddMarkerToAll_Implementation(const TArray<APickup*>& QuestItems)
{
	// 이 코드는 모든 클라이언트에서 실행됩니다.
	if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
	{
		if (UNS_UIManager* UIManager = GI->GetUIManager())
		{
			if (UNS_PlayerHUD* PlayerHUD = UIManager->GetPlayerHUDWidget())
			{
				for (APickup* Item : QuestItems)
				{
					if (Item)
					{
						PlayerHUD->SetYeddaItem(Item);
					}
				}
			}
		}
	}
}

void APickup::Multicast_RemoveMarkerFromAll_Implementation()
{
	// 이 코드는 모든 클라이언트에서 실행됩니다.
	if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
	{
		if (UNS_UIManager* UIManager = GI->GetUIManager())
		{
			if (UNS_PlayerHUD* PlayerHUD = UIManager->GetPlayerHUDWidget())
			{
				PlayerHUD->DeleteCompasItem(this);
			}
		}
	}
}

void APickup::OnHUDReady(UNS_PlayerHUD* PlayerHUD)
{
	if (IsValid(PlayerHUD))
	{
		PlayerHUD->SetYeddaItem(this);

		if (UNS_GameInstance* GI = GetGameInstance<UNS_GameInstance>())
		{
			if (UNS_UIManager* UIManager = GI->GetUIManager())
			{
				UIManager->OnPlayerHUDReady.RemoveDynamic(this, &APickup::OnHUDReady);
			}
		}
	}
}