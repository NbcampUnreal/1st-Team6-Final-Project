// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NS_PlayerHUD.generated.h"

class UNS_CompassElement;
class APickup;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_PlayerHUD : public UUserWidget
{
	GENERATED_BODY()
public:
    virtual void NativeConstruct() override;
    void ShowWidget();
    void HideWidget();
    void SetYeddaItem(APickup* YeddaItem);

    void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	void DeleteCompasItem(APickup* DeleteItem);


    UPROPERTY(meta = (BindWidget))
    class UNS_QuickSlotPanel* NS_QuickSlotPanel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite,  Category = "TEST")
    int32 TEST_CNT;

    //UPROPERTY(EditDefaultsOnly, Category = "UI")
    //UNS_CompassElement* NS_CompassElement;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UNS_CompassElement> NS_CompassElementClass;
    /// Script / UMGEditor.WidgetBlueprint'/Game/SurvivalGameKitV2/Blueprints/Widgets/WBP_CompassElement.WBP_CompassElement'
protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TArray<UTexture2D*> TextureArray;

    UPROPERTY(meta = (BindWidget))
    class UNS_CircleProgressBar* WBP_StatusProgressbar_Health;
    UPROPERTY(meta = (BindWidget))
    class UNS_CircleProgressBar* WBP_StatusProgressbar_Stamina;
    UPROPERTY(meta = (BindWidget))
    class UNS_CircleProgressBar* WBP_StatusProgressbar_Hunger;
    UPROPERTY(meta = (BindWidget))
    class UNS_CircleProgressBar* WBP_StatusProgressbar_Thirst;
    UPROPERTY(meta = (BindWidget))
    class UNS_CircleProgressBar* WBP_StatusProgressbar_Fatigue;

    UPROPERTY(meta = (BindWidget))
    class UProgressBar* ProgressBar_Health;
    UPROPERTY(meta = (BindWidget))
    class UProgressBar* ProgressBar_Stamina;

    UPROPERTY(meta = (BindWidget))
    class UScrollBox* ScrollBox_Compass;
    TArray<class UNS_CompassElement*> CompassTextArray;//UTextBlock UNS_CompassElement

private:
    FTimerHandle UpdatePlayerStausHandle;
    APlayerController* PlayerController;
    TArray<APickup*> YeddaItemArray;

    bool testcheck = false;
    int32 PrvFinalIdx = 0;
  
    // 각 나침반 눈금의 시작 위치(픽셀 오프셋)를 저장할 배열
    TArray<float> CompassElementOffsets;

    // 각 나침반 눈금의 너비(픽셀)를 저장할 배열
    TArray<float> CompassElementWidths;

    UPROPERTY()
    class ANS_PlayerCharacterBase* CachedPlayerCharacter = nullptr;

    UPROPERTY()
    int32 LastHighlightedIndex = -1; // 마지막으로 강조한 인덱스 캐싱
};
