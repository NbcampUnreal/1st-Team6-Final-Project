// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NS_NumericalTextBox.generated.h"

class UEditableTextBox;
class USlider;
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_NumericalTextBox : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UEditableTextBox* EditableTextBox;

	UPROPERTY( BlueprintReadWrite) //블루프린트에서 생성해놓은 Slider때문에 애러가 나서 임시변수로 해놓은거임.
	USlider* TempSlider;
};
