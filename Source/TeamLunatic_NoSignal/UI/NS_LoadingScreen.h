// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NS_LoadingScreen.generated.h"

/**
 * 
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_LoadingScreen : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	bool IsEndLoadingScreen() const { return bIsFinished; }
	//UFUNCTION()
	void UpdateProgress();  

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ProgressBar_Loading;

	float FakeProgress = 0.f;
	bool bIsLevelLoadComplete = false;
	bool bIsFinished = false;
	FTimerHandle LoadingTickHandle;
};
