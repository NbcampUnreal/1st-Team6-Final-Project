// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NS_CommonType.h"
#include "NS_MenuButtonWidget.generated.h"

class UButton;
class UTextBlock;
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_MenuButtonWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	virtual void NativeConstruct() override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	EWidgetToggleType TgType;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UButton* RootButton;

protected:

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UTextBlock* Text;


	UFUNCTION()
	void OnButtonClicked();
};
