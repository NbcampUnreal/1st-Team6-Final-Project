#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NS_PlayerWidget.generated.h"

struct FInteractableData;
class UTextBlock;
class UImage;
class UNS_InteractionPanel;

// 플레이어의 UI를 담당하는 위젯

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_PlayerWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    virtual void NativeConstruct() override;
    
    // 상호작용 위젯을 표시
    void ShowInteractionWidget();
    
    // 상호작용 위젯을 숨김
    void HideInteractionWidget();
    
    // 상호작용 위젯을 업데이트
    void UpdateInteractionWidget(const FInteractableData* InteractableData);

    // 플레이어의 체력을 표시하는 텍스트 블록 위젯
    UPROPERTY(meta = (BindWidget))
    UTextBlock* HealthText;

    // 플레이어의 스태미나를 표시하는 텍스트 블록 위젯
    UPROPERTY(meta = (BindWidget))
    UTextBlock* StaminaText;

    // 게임 내 크로스헤어를 표시하는 이미지 위젯
    UPROPERTY(meta = (BindWidget))
    UImage* Crosshair;
    
    // 상호작용 위젯 클래스
    UPROPERTY(EditDefaultsOnly, Category = "Widgets")
    TSubclassOf<class UNS_InteractionPanel> InteractionWidgetClass;
    
    // 상호작용 위젯 인스턴스 
    UPROPERTY()
    UNS_InteractionPanel* InteractionWidget;
};