// NS_PlayerWidget.h - 단순화된 플레이어 UI 위젯

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NS_PlayerWidget.generated.h"

/**
 * @brief 플레이어의 UI를 담당하는 위젯입니다.
 *        체력, 스태미나, 크로스헤어 등 게임 플레이 중 필요한 정보를 표시합니다.
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_PlayerWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /**
     * @brief 위젯이 생성될 때 호출되는 초기화 함수입니다.
     *        주로 위젯의 초기 상태 설정 및 바인딩에 사용됩니다.
     */
    virtual void NativeConstruct() override;
    
    /**
     * @brief 플레이어의 현재 체력과 최대 체력을 업데이트하여 UI에 표시합니다.
     * @param CurrentHealth 현재 플레이어의 체력입니다.
     * @param MaxHealth 플레이어의 최대 체력입니다.
     */
    void UpdateHealth(int32 CurrentHealth, int32 MaxHealth);

    /**
     * @brief 플레이어의 현재 스태미나와 최대 스태미나를 업데이트하여 UI에 표시합니다.
     * @param CurrentStamina 현재 플레이어의 스태미나입니다.
     * @param MaxStamina 플레이어의 최대 스태미나입니다.
     */
    void UpdateStamina(int32 CurrentStamina, int32 MaxStamina);

    /**
     * @brief 크로스헤어의 가시성을 설정합니다.
     * @param bVisible 크로스헤어를 보이게 할지(true) 숨기게 할지(false) 여부입니다.
     */
    void SetCrosshairVisibility(bool bVisible); 

    /**
     * @brief 플레이어의 체력을 표시하는 텍스트 블록 위젯입니다.
     *        블루프린트에서 바인딩됩니다.
     */
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* HealthText;

    /**
     * @brief 플레이어의 스태미나를 표시하는 텍스트 블록 위젯입니다.
     *        블루프린트에서 바인딩됩니다.
     */
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* StaminaText;

    /**
     * @brief 게임 내 크로스헤어를 표시하는 이미지 위젯입니다.
     *        블루프린트에서 바인딩됩니다.
     */
    UPROPERTY(meta = (BindWidget))
    class UImage* Crosshair;
};