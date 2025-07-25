#include "Character/DeathBox/NS_DeathBox.h"
#include "Components/StaticMeshComponent.h"
#include "Item/NS_InventoryBaseItem.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Character/Components/NS_InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"

ANS_DeathBox::ANS_DeathBox()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	BoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxMesh"));
	RootComponent = BoxMesh;
	
	BoxMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxMesh->SetCollisionResponseToAllChannels(ECR_Block);
}

void ANS_DeathBox::BeginPlay()
{
	Super::BeginPlay();
	
	// 일정 시간 후 자동 삭제
	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(DestroyTimerHandle, [this]()
		{
			Destroy();
		}, DestroyDelay, false);
	}
}

void ANS_DeathBox::StoreItems(const TArray<UNS_InventoryBaseItem*>& Items)
{
	if (!HasAuthority()) return;
	
	StoredItems.Empty();
	for (UNS_InventoryBaseItem* Item : Items)
	{
		if (IsValid(Item))
		{
			StoredItems.Add(Item);
		}
	}
}

void ANS_DeathBox::BeginFocus()
{
	if (BoxMesh)
	{
		BoxMesh->SetRenderCustomDepth(true);
	}
}

void ANS_DeathBox::EndFocus()
{
	if (BoxMesh)
	{
		BoxMesh->SetRenderCustomDepth(false);
	}
}

void ANS_DeathBox::Interact_Implementation(AActor* InteractingActor)
{
	if (!HasAuthority()) return;
	
	ANS_PlayerCharacterBase* Player = Cast<ANS_PlayerCharacterBase>(InteractingActor);
	if (!Player) return;
	
	UNS_InventoryComponent* PlayerInventory = Player->GetInventory();
	if (!PlayerInventory) return;
	
	// 모든 아이템을 플레이어 인벤토리에 추가 시도
	TArray<UNS_InventoryBaseItem*> RemainingItems;
	
	for (UNS_InventoryBaseItem* Item : StoredItems)
	{
		if (IsValid(Item))
		{
			FItemAddResult Result = PlayerInventory->HandleAddItem(Item);
			
			// 추가되지 않은 아이템은 다시 저장
			if (Result.OperationResult == EItemAddResult::TAR_NoItemAdded)
			{
				RemainingItems.Add(Item);
			}
		}
	}
	
	StoredItems = RemainingItems;
	
	// 모든 아이템이 가져가졌으면 상자 삭제
	if (StoredItems.Num() == 0)
	{
		Destroy();
	}
}

void ANS_DeathBox::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANS_DeathBox, StoredItems);
}