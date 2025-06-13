// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NS_PlayerHUD.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_PlayerHUD : public UUserWidget
{
	GENERATED_BODY()
public:
    virtual void NativeConstruct() override;
    void ShowWidget();
    void HideWidget();

    void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

    UPROPERTY(meta = (BindWidget))
    class UNS_QuickSlotPanel* NS_QuickSlotPanel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite,  Category = "TEST")
    int32 TEST_CNT;

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
    TArray<class UTextBlock*> CompassTextArray;
private:
    class ANS_PlayerCharacterBase* CachedMyCharacter;
    FTimerHandle UpdatePlayerStausHandle;

    APlayerController* PlayerController;
    bool testcheck = false;
};
