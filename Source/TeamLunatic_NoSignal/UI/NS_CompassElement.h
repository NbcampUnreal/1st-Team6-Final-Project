// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NS_CompassElement.generated.h"

/**
 * 
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_CompassElement : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	float GetSizeBoxWideth();
	void SetTextScale(float Scale);
	bool IsNumericString(const FString& Str);
	void SetDirLetter(const FString& DirLetter);
	
	void ReturnTextScale();

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UTextBlock* TextDir;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UTextBlock* TextGraduation;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class USizeBox* TextSizeBox;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UImage* Image_Arrow;

	bool bDirLetter = false;
};
