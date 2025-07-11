#include "UI/InGame/NS_NearbyItemsWidget.h"
#include "Components/ScrollBox.h"
#include "UI/InGame/NS_NearbyItemEntry.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "World/Pickup.h"

void UNS_NearbyItemsWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	UE_LOG(LogTemp, Warning, TEXT("NS_NearbyItemsWidget::NativeConstruct 호출됨"));
	
	// 인터랙션 컴포넌트 찾기
	FindInteractionComponent();
	
	// 위젯 크기 확인
	FVector2D Size = GetDesiredSize();
	UE_LOG(LogTemp, Warning, TEXT("NS_NearbyItemsWidget 크기: %f x %f"), Size.X, Size.Y);
	
	// 위젯이 화면에 보이도록 크기 설정
	if (Size.X < 100 || Size.Y < 100)
	{
		UE_LOG(LogTemp, Warning, TEXT("NS_NearbyItemsWidget 크기가 너무 작음, 기본 크기로 설정"));
		SetDesiredSizeInViewport(FVector2D(300, 400));
	}
}

void UNS_NearbyItemsWidget::UpdateItemsList(const TArray<FNearbyItemInfo>& NearbyItems)
{
	UE_LOG(LogTemp, Warning, TEXT("UpdateItemsList 호출됨 - 아이템 수: %d"), NearbyItems.Num());
	
	// 스크롤 박스가 없으면 종료
	if (!ItemsScrollBox)
	{
		UE_LOG(LogTemp, Error, TEXT("ItemsScrollBox가 유효하지 않음! 블루프린트에서 이름이 정확한지 확인하세요."));
		return;
	}
	
	// 기존 항목 제거
	ItemsScrollBox->ClearChildren();
	
	// 주변 아이템이 없으면 종료
	if (NearbyItems.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("주변 아이템이 없습니다."));
		return;
	}
	
	// 아이템 항목 위젯 클래스가 없으면 종료
	if (!ItemEntryClass)
	{
		UE_LOG(LogTemp, Error, TEXT("ItemEntryClass가 유효하지 않음! 블루프린트에서 설정해야 합니다."));
		return;
	}
	
	// 각 아이템에 대한 항목 위젯 생성
	int32 ItemCount = 0;
	for (const FNearbyItemInfo& ItemInfo : NearbyItems)
	{
		// 아이템이 유효한지 확인
		if (!ItemInfo.Item || !ItemInfo.ItemActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("유효하지 않은 아이템 정보 건너뚼"));
			continue;
		}
		
		// 항목 위젯 생성
		UNS_NearbyItemEntry* ItemEntry = CreateWidget<UNS_NearbyItemEntry>(this, ItemEntryClass);
		if (ItemEntry)
		{
			UE_LOG(LogTemp, Warning, TEXT("항목 위젯 생성 성공"));
			
			// 항목 위젯 초기화
			ItemEntry->SetItemInfo(ItemInfo);
			ItemEntry->SetParentWidget(this);
			
			// 스크롤 박스에 추가
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

void UNS_NearbyItemsWidget::RequestPickupItem(APickup* ItemActor)
{
	// 인터랙션 컴포넌트가 없으면 찾기
	if (!InteractionComponent)
	{
		FindInteractionComponent();
	}
	
	// 인터랙션 컴포넌트가 있으면 아이템 획득 요청
	if (InteractionComponent)
	{
		InteractionComponent->PickupNearbyItem(ItemActor);
	}
}

void UNS_NearbyItemsWidget::FindInteractionComponent()
{
	// 플레이어 컨트롤러 가져오기
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return;
	}
	
	// 플레이어 캐릭터 가져오기
	ANS_PlayerCharacterBase* PlayerCharacter = Cast<ANS_PlayerCharacterBase>(PC->GetPawn());
	if (!PlayerCharacter)
	{
		return;
	}
	
	// 인터랙션 컴포넌트 가져오기
	InteractionComponent = PlayerCharacter->GetInteractionComponent();
}