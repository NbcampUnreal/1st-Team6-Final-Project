// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/NS_BaseWidgetInterface.h"
#include "NS_LogoScreenWidget.generated.h"

/**
 * 
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_LogoScreenWidget : public UUserWidget, public INS_BaseWidgetInterface
{
	GENERATED_BODY()
public:
	// 첫 번째 로고 애니메이션
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* ShowLogo;
	
	// 두 번째 로고 애니메이션
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* ShowSecondLogo;
	
	// 메뉴로 전환하는 델리게이트
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLogosFinished);
	UPROPERTY(BlueprintAssignable, Category = "Logo")
	FOnLogosFinished OnLogosFinished;
	
protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnShowLogoFinished();
	
	UFUNCTION()
	void OnShowSecondLogoFinished();
	
	// 두 번째 로고 애니메이션 시작
	void StartSecondLogoAnimation();
};
