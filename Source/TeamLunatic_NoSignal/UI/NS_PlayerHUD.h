// NS_PlayerHUD.h - 플레이어의 게임 내 HUD(Head-Up Display) 위젯을 정의하는 헤더 파일

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NS_PlayerHUD.generated.h"

class APickup;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_PlayerHUD : public UUserWidget
{
    GENERATED_BODY()
public:
    // 위젯이 생성될 때 호출되는 함수
    virtual void NativeConstruct() override;
    
    // HUD 위젯을 화면에 표시
    void ShowWidget();
    
    // HUD 위젯을 화면에서 숨김
    void HideWidget();
    
    // 특정 아이템을 나침반에 표시 (현재 사용하지 않음)
    void SetYeddaItem(APickup* YeddaItem); 
    
    // 매 프레임마다 호출되는 함수
    void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
    
    // 나침반에서 아이템 제거 (현재 사용하지 않음)
    void DeleteCompasItem(APickup* DeleteItem);
    
    // 팁 텍스트 설정
    void SetTipText(const FText& NewText);
    
    // 조준점 표시/숨김 설정
    UFUNCTION(BlueprintCallable, Category = "UI")
    void SetCrosshairVisibility(bool bVisible);

    // 퀵슬롯 패널 위젯
    UPROPERTY(meta = (BindWidget))
    class UNS_QuickSlotPanel* NS_QuickSlotPanel;

    // 테스트용 카운터 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TEST")
    int32 TEST_CNT;

    // 나침반 요소 위젯 클래스
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UNS_CompassElement> NS_CompassElementClass;

    // 체력 프로그레스 바
    UPROPERTY(meta = (BindWidget))
    class UProgressBar* ProgressBar_Health;

    // 체력 프로그레스 바 BG
    UPROPERTY(meta = (BindWidget))
    class UProgressBar* ProgressBarBG_Health;

    // 스태미너 프로그레스 바
    UPROPERTY(meta = (BindWidget))
    class UProgressBar* ProgressBar_Stamina;

    // 스태미너 프로그레스 바 BG
    UPROPERTY(meta = (BindWidget))
    class UProgressBar* ProgressBarBG_Stamina;

    // 나침반 스크롤 박스
    UPROPERTY(meta = (BindWidget))
    class UScrollBox* ScrollBox_Compass;

    // 팁 텍스트 블록
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TipText;
    
    // 조준점 이미지
    UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "UI")
    class UImage* Crosshair;

private:
    // 플레이어 상태 업데이트를 위한 타이머 핸들
    FTimerHandle UpdatePlayerStausHandle;
    
    // 캐싱된 플레이어 캐릭터 참조
    UPROPERTY()
    class ANS_PlayerCharacterBase* CachedPlayerCharacter;
    
    // 나침반 텍스트 요소 배열
    UPROPERTY()
    TArray<class UNS_CompassElement*> CompassTextArray;
    
    // 초기화 완료 플래그
    bool testcheck = false;

    float CurrentHpPercent;
    float CurrentStPercent;

    float BGHpPercent;
    float BGStPercent;
};