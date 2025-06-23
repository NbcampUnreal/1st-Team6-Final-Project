#include "World/Pickup.h"
#include "Engine/DataTable.h"
#include "Item/NS_ItemDataStruct.h"
#include "Item/NS_BaseWeapon.h"
#include "Item/NS_InventoryBaseItem.h"
#include "Inventory/InventoryComponent.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Item/NS_BaseMagazine.h"
#include "Inventory/QSlotCom/NS_QuickSlotComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFlow/NS_GameInstance.h"
#include "UI/NS_UIManager.h"
#include "UI/NS_PlayerHUD.h"
#include "GameFlow/NS_PlayerState.h"

APickup::APickup()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicatingMovement(true);

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>("PickupMesh");
	PickupMesh->SetSimulatePhysics(true);
	SetRootComponent(PickupMesh);
}

void APickup::BeginPlay()
{
	Super::BeginPlay();

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

	UpdateInteractableData();
}

void APickup::OnRep_ReplicatedItemData()
{
	if (!ItemReference)
	{
		ItemReference = NewObject<UNS_InventoryBaseItem>(GetWorld(), UNS_InventoryBaseItem::StaticClass());
		if (!ItemReference)
		{
			UE_LOG(LogTemp, Error, TEXT("[OnRep_ReplicatedItemData] ItemReference 생성 실패"));
			return;
		}
	}

	ItemReference->ItemDataRowName = ReplicatedItemData.ItemDataRowName;
	ItemReference->ItemsDataTable = ItemDataTable;
	ItemReference->TextData = ReplicatedItemData.ItemTextData;
	ItemReference->NumericData = ReplicatedItemData.ItemNumericData;
	ItemReference->AssetData = ReplicatedItemData.ItemAssetData;
	ItemReference->WeaponData = ReplicatedItemData.WeaponData;
	ItemReference->WeaponType = ReplicatedItemData.WeaponType;
	ItemReference->ItemType = ReplicatedItemData.ItemType;

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
			UE_LOG(LogTemp, Error, TEXT("[TakePickup] ItemReference is null or invalid. GC로 수거되었을 가능성 있음."));
			return;
		}
		if (UInventoryComponent* PlayerInventory = Taker->GetInventory())
		{
			const FItemAddResult AddResult = PlayerInventory->HandleAddItem(ItemReference);

			if (AddResult.OperationResult == EItemAddResult::TAR_AllItemAdded || AddResult.OperationResult == EItemAddResult::TAR_PartialAmountItemAdded)
			{
				UNS_PlayerHUD* PlayerHUD = nullptr;
				if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
				{
					if (UNS_UIManager* UIManager = GI->GetUIManager())
					{
						PlayerHUD = UIManager->GetPlayerHUDWidget();
					}
				}

				if (PlayerHUD)
				{
					if (ItemReference->ItemDataRowName == FName("Memo"))
					{
						TArray<AActor*> FoundPickups;
						UGameplayStatics::GetAllActorsOfClass(GetWorld(), APickup::StaticClass(), FoundPickups);

						for (AActor* PickupActor : FoundPickups)
						{
							APickup* QuestPickup = Cast<APickup>(PickupActor);
							if (!QuestPickup || !QuestPickup->GetItemData()) continue;

							FName ItemID = QuestPickup->GetItemData()->ItemDataRowName;

							const TArray<FName> TargetNoteIDs = {
								FName("One"), FName("Two"), FName("Three"), FName("Four"), FName("Five")
							};

							if (TargetNoteIDs.Contains(ItemID))
							{
								PlayerHUD->SetYeddaItem(QuestPickup);
							}
						}
					}

					PlayerHUD->DeleteCompasItem(this);
				}
			}

			switch (AddResult.OperationResult)
			{
			case EItemAddResult::TAR_NoItemAdded:
				break;
			case EItemAddResult::TAR_PartialAmountItemAdded:
				if (ReplicatedItemData.ItemAssetData.GetSound)
				{
					UGameplayStatics::PlaySound2D(this, ReplicatedItemData.ItemAssetData.GetSound);
				}
				UpdateInteractableData();
				if (UInteractionComponent* InteractionComp = Taker->GetInteractionComponent())
				{
					InteractionComp->UpdateInteractionWidget();
				}
				break;
			case EItemAddResult::TAR_AllItemAdded:
				if (ReplicatedItemData.ItemAssetData.GetSound)
				{
					UGameplayStatics::PlaySound2D(this, ReplicatedItemData.ItemAssetData.GetSound);
				}
				Destroy();
				break;
			}

			UE_LOG(LogTemp, Warning, TEXT("[TakePickup] AddResult: %d, Message: %s"),
				(int32)AddResult.OperationResult,
				*AddResult.ResultMessage.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Player Inventory Component is null"));
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

	UE_LOG(LogTemp, Warning, TEXT("APickup::TryAssignToHUD "));

	// HUD가 아직 생성되지 않았을 경우 재시도
	FTimerHandle RetryHandle;
	GetWorldTimerManager().SetTimer(
		RetryHandle,
		this,
		&APickup::TryAssignToHUD,
		0.2f,
		false
	);
}
#endif

void APickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APickup, ReplicatedItemData);
}
