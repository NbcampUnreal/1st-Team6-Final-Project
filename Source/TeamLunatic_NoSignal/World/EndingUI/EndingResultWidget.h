// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndingResultWidget.generated.h"

class UTextBlock;
class UVerticalBox;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UEndingResultWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetPlayerResultLists(const TArray<FString>& SuccessList, const TArray<FString>& FailList);

	void SetEndingType(FName EndingType);
protected:
	// 바인딩된 성공/실패 표시 영역
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* SuccessPlayer;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* FailPlayer;

	UPROPERTY(meta = (BindWidget))
	class UImage* RadioEndingImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* CarEndingImage;

	UPROPERTY(meta = (BindWidget))
	class UButton* ReturnToLobbyButton;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* RadionEndingText;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CarEndingText;
};
