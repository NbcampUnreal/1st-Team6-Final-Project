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

	void LevelLoadComplete()
	{
		bIsLevelLoadComplete = true;
	}
	void FakeUpdateProgress();
	//UFUNCTION()
	void UpdateProgress();

	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ProgressBar_Loading;

	float FakeProgress = 0.f;
	float FakeProgressMax = 15.f; // 15초 동안 로딩이 진행된다고 가정
	bool bIsLevelLoadComplete = false;
	FTimerHandle LoadingTickHandle;

};
