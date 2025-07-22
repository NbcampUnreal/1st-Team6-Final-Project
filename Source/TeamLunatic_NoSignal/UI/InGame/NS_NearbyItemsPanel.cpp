#include "UI/InGame/NS_NearbyItemsPanel.h"
#include "Components/ScrollBox.h"
#include "UI/InGame/NS_NearbyItemEntry.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "World/Pickup.h"

void UNS_NearbyItemsPanel::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 인터랙션 컴포넌트 찾기 및 할당을 시도
	FindInteractionComponent();
	
	// 위젯의 현재 크기를 확인합니다.
	FVector2D Size = GetDesiredSize();
	
	// 위젯의 크기가 너무 작으면 기본 크기로 설정하여 화면에 제대로 보이도록
	if (Size.X < 100 || Size.Y < 100)
	{
		SetDesiredSizeInViewport(FVector2D(300, 400));
	}
}

// 주변 아이템 목록을 업데이트하여 UI에 표시
void UNS_NearbyItemsPanel::UpdateItemsList(const TArray<FNearbyItemInfo>& NearbyItems)
{
	// ItemsScrollBox가 유효하지 않으면 오류를 로깅하고 함수를 종료
	if (!ItemsScrollBox)
	{
		return;
	}
	
	// 기존에 스크롤 박스에 추가된 모든 자식 위젯을 제거
	ItemsScrollBox->ClearChildren();
	
	// 주변 아이템이 없으면 경고를 로깅하고 함수를 종료
	if (NearbyItems.Num() == 0)
	{
		return;
	}
	
	// ItemEntryClass(아이템 항목 위젯 클래스)가 유효하지 않으면 오류를 로깅하고 함수를 종료
	if (!ItemEntryClass)
	{
		return;
	}
	
	// 각 아이템 정보에 대해 항목 위젯을 생성하고 스크롤 박스에 추가
	int32 ItemCount = 0;
	for (const FNearbyItemInfo& ItemInfo : NearbyItems)
	{
		// 아이템 정보가 유효한지 확인
		if (!ItemInfo.Item || !ItemInfo.ItemActor)
		{
			continue;
		}
		
		// 새로운 아이템 항목 위젯을 생성
		UNS_NearbyItemEntry* ItemEntry = CreateWidget<UNS_NearbyItemEntry>(this, ItemEntryClass);
		if (ItemEntry)
		{
			
			// 생성된 항목 위젯에 아이템 정보를 설정하고 부모 위젯을 지정함
			ItemEntry->SetItemInfo(ItemInfo);
			ItemEntry->SetParentWidget(this);
			
			// 스크롤 박스에 항목 위젯을 추가
			ItemsScrollBox->AddChild(ItemEntry);
			ItemCount++;
		}
	}
}

// 특정 아이템을 획득하도록 요청
void UNS_NearbyItemsPanel::RequestPickupItem(APickup* ItemActor)
{
	// InteractionComponent가 아직 할당되지 않았다면 찾아서 할당을 시도
	if (!InteractionComponent)
	{
		FindInteractionComponent();
	}
	
	// InteractionComponent가 유효하면 해당 아이템을 획득하도록 요청
	if (InteractionComponent)
	{
		InteractionComponent->PickupNearbyItem(ItemActor);
	}
}

// 플레이어 캐릭터에서 상호작용 컴포넌트를 찾아 할당하는 함수

void UNS_NearbyItemsPanel::FindInteractionComponent()
{
	// 현재 위젯을 소유한 플레이어 컨트롤러를 가져옴
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("[NS_NearbyItemsPanel] PlayerController를 찾을 수 없음"));
		return;
	}
	
	ANS_PlayerCharacterBase* PlayerCharacter = Cast<ANS_PlayerCharacterBase>(PC->GetPawn());
	if (!PlayerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("[NS_NearbyItemsPanel] PlayerCharacter를 찾을 수 없음"));
		return;
	}
	
	// 플레이어 캐릭터로부터 상호작용 컴포넌트를 가져와 할당
	InteractionComponent = PlayerCharacter->GetInteractionComponent();
	if (InteractionComponent)
	{
		UE_LOG(LogTemp, Log, TEXT("[NS_NearbyItemsPanel] InteractionComponent 찾기 성공"));
		
		// 델리게이트 바인딩
		InteractionComponent->OnNearbyItemsUpdated.AddDynamic(this, &UNS_NearbyItemsPanel::OnNearbyItemsUpdated);
		UE_LOG(LogTemp, Log, TEXT("[NS_NearbyItemsPanel] OnNearbyItemsUpdated 델리게이트 바인딩 완료"));
		
		// 초기 아이템 목록 업데이트
		TArray<FNearbyItemInfo> CurrentItems = InteractionComponent->GetNearbyItems();
		UpdateItemsList(CurrentItems);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[NS_NearbyItemsPanel] InteractionComponent를 찾을 수 없음"));
	}
}

// 주변 아이템이 업데이트될 때 호출되는 함수
void UNS_NearbyItemsPanel::OnNearbyItemsUpdated()
{
	UE_LOG(LogTemp, Warning, TEXT("[NS_NearbyItemsPanel] OnNearbyItemsUpdated 델리게이트 호출됨!"));
	
	if (InteractionComponent)
	{
		TArray<FNearbyItemInfo> CurrentItems = InteractionComponent->GetNearbyItems();
		UpdateItemsList(CurrentItems);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[NS_NearbyItemsPanel] InteractionComponent가 null입니다!"));
	}
}