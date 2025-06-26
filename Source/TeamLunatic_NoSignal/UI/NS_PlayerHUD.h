// NS_PlayerHUD.h

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
    void SetTipText(const FText& NewText);

    UPROPERTY(meta = (BindWidget))
    class UNS_QuickSlotPanel* NS_QuickSlotPanel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TEST")
    int32 TEST_CNT;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UNS_CompassElement> NS_CompassElementClass;

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

    UPROPERTY()
    TArray<TObjectPtr<class UNS_CompassElement>> CompassTextArray;

    UPROPERTY(meta = (BindWidget))
    class UEditableTextBox* TipText;

private:
    FTimerHandle UpdatePlayerStausHandle;
    APlayerController* PlayerController;

    bool testcheck = false;
    int32 PrvFinalIdx = 0;

    TArray<float> CompassElementOffsets;
    TArray<float> CompassElementWidths;

    UPROPERTY()
    class ANS_PlayerCharacterBase* CachedPlayerCharacter = nullptr;

    UPROPERTY()
    int32 LastHighlightedIndex = -1;
};