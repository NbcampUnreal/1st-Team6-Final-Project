#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NS_HostNewGameServerR.generated.h"

class UTextBlock;
class UButton;
class UEditableTextBox;
class USpacer;
class UBorder;
class UComboBoxString;
class UNS_AreYouSureMenu;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_HostNewGameServerR : public UUserWidget
{
    GENERATED_BODY()

public:
    FString LoadMapName;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UBorder* Backround;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UTextBlock* Title;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UEditableTextBox* SaveNameEntryBox;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    USpacer* SelectMapSpacer_1;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UComboBoxString* ComboBoxString;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    USpacer* SelectMapSpacer;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UButton* CreateServerButton;

protected:
    virtual void NativeConstruct() override;

    FString GetSaveSlotName() const;

    void StartGame();

    UFUNCTION()
    void OnCreateServerButtonClicked();
};
