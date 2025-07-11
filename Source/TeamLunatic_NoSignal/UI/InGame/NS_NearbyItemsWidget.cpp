#include "UI/InGame/NS_NearbyItemsWidget.h"
#include "Components/ScrollBox.h"
#include "UI/InGame/NS_NearbyItemEntry.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "World/Pickup.h"

/**
 * @brief 위젯이 생성될 때 호출되는 초기화 함수입니다.
 *        주로 위젯의 초기 상태 설정 및 바인딩에 사용됩니다.
 */
void UNS_NearbyItemsWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	UE_LOG(LogTemp, Warning, TEXT("NS_NearbyItemsWidget::NativeConstruct 호출됨"));
	
	// 인터랙션 컴포넌트 찾기 및 할당을 시도합니다.
	FindInteractionComponent();
	
	// 위젯의 현재 크기를 확인합니다.
	FVector2D Size = GetDesiredSize();
	UE_LOG(LogTemp, Warning, TEXT("NS_NearbyItemsWidget 크기: %f x %f"), Size.X, Size.Y);
	
	// 위젯의 크기가 너무 작으면 기본 크기로 설정하여 화면에 제대로 보이도록 합니다.
	if (Size.X < 100 || Size.Y < 100)
	{
		UE_LOG(LogTemp, Warning, TEXT("NS_NearbyItemsWidget 크기가 너무 작음, 기본 크기로 설정"));
		SetDesiredSizeInViewport(FVector2D(300, 400));
	}
}

/**
 * @brief 주변 아이템 목록을 업데이트하여 UI에 표시합니다.
 * @param NearbyItems 주변에 있는 아이템들의 정보가 담긴 배열입니다.
 *        이 함수는 스크롤 박스의 내용을 지우고 새로운 아이템 항목들을 추가합니다.
 */
void UNS_NearbyItemsWidget::UpdateItemsList(const TArray<FNearbyItemInfo>& NearbyItems)
{
	UE_LOG(LogTemp, Warning, TEXT("UpdateItemsList 호출됨 - 아이템 수: %d"), NearbyItems.Num());
	
	// ItemsScrollBox가 유효하지 않으면 오류를 로깅하고 함수를 종료합니다.
	if (!ItemsScrollBox)
	{
		UE_LOG(LogTemp, Error, TEXT("ItemsScrollBox가 유효하지 않음! 블루프린트에서 이름이 정확한지 확인하세요."));
		return;
	}
	
	// 기존에 스크롤 박스에 추가된 모든 자식 위젯을 제거합니다.
	ItemsScrollBox->ClearChildren();
	
	// 주변 아이템이 없으면 경고를 로깅하고 함수를 종료합니다.
	if (NearbyItems.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("주변 아이템이 없습니다."));
		return;
	}
	
	// ItemEntryClass(아이템 항목 위젯 클래스)가 유효하지 않으면 오류를 로깅하고 함수를 종료합니다.
	if (!ItemEntryClass)
	{
		UE_LOG(LogTemp, Error, TEXT("ItemEntryClass가 유효하지 않음! 블루프린트에서 설정해야 합니다."));
		return;
	}
	
	// 각 아이템 정보에 대해 항목 위젯을 생성하고 스크롤 박스에 추가합니다.
	int32 ItemCount = 0;
	for (const FNearbyItemInfo& ItemInfo : NearbyItems)
	{
		// 아이템 정보가 유효한지 확인합니다.
		if (!ItemInfo.Item || !ItemInfo.ItemActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("유효하지 않은 아이템 정보 건너뜀"));
			continue;
		}
		
		// 새로운 아이템 항목 위젯을 생성합니다.
		UNS_NearbyItemEntry* ItemEntry = CreateWidget<UNS_NearbyItemEntry>(this, ItemEntryClass);
		if (ItemEntry)
		{
			UE_LOG(LogTemp, Warning, TEXT("항목 위젯 생성 성공"));
			
			// 생성된 항목 위젯에 아이템 정보를 설정하고 부모 위젯을 지정합니다.
			ItemEntry->SetItemInfo(ItemInfo);
			ItemEntry->SetParentWidget(this);
			
			// 스크롤 박스에 항목 위젯을 추가합니다.
			ItemsScrollBox->AddChild(ItemEntry);
			ItemCount++;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("항목 위젯 생성 실패!"));
		}
	}
	
	UE_LOG(LogTemp, Warning, TEXT("총 %d개의 아이템 항목이 추가됨"), ItemCount);
}

/**
 * @brief 특정 아이템을 획득하도록 요청합니다.
 * @param ItemActor 획득할 아이템 액터에 대한 포인터입니다.
 *        블루프린트에서 호출 가능하며, 주로 아이템 항목 위젯에서 클릭 이벤트 발생 시 사용됩니다.
 */
void UNS_NearbyItemsWidget::RequestPickupItem(APickup* ItemActor)
{
	// InteractionComponent가 아직 할당되지 않았다면 찾아서 할당을 시도합니다.
	if (!InteractionComponent)
	{
		FindInteractionComponent();
	}
	
	// InteractionComponent가 유효하면 해당 아이템을 획득하도록 요청합니다.
	if (InteractionComponent)
	{
		InteractionComponent->PickupNearbyItem(ItemActor);
	}
}

/**
 * @brief 플레이어 캐릭터에서 상호작용 컴포넌트를 찾아 할당하는 함수입니다.
 *        NativeConstruct에서 호출되어 InteractionComponent를 초기화합니다.
 */
void UNS_NearbyItemsWidget::FindInteractionComponent()
{
	// 현재 위젯을 소유한 플레이어 컨트롤러를 가져옵니다.
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return;
	}
	
	// 플레이어 컨트롤러로부터 폰(캐릭터)을 가져와 ANS_PlayerCharacterBase로 캐스팅합니다.
	ANS_PlayerCharacterBase* PlayerCharacter = Cast<ANS_PlayerCharacterBase>(PC->GetPawn());
	if (!PlayerCharacter)
	{
		return;
	}
	
	// 플레이어 캐릭터로부터 상호작용 컴포넌트를 가져와 할당합니다.
	InteractionComponent = PlayerCharacter->GetInteractionComponent();
}