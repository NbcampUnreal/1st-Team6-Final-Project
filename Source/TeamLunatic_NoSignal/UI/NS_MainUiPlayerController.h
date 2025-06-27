// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
#include "NS_MainUiPlayerController.generated.h"

class UNS_MainMenu;

/**
 * 메인 UI 플레이어 컨트롤러 클래스
 * 게임 시작 메뉴 및 메인 메뉴 화면에서 사용되는 플레이어 컨트롤러입니다.
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_MainUiPlayerController : public APlayerController
{
    GENERATED_BODY()

protected:
    /** 게임 시작 시 호출되는 함수 */
    virtual void BeginPlay() override;

    /** 메인 메뉴 위젯 클래스 */
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UNS_MainMenu> NS_MainMenuWidgetClass;

    /** 메인 메뉴 위젯 인스턴스 */
    UPROPERTY()
    UNS_MainMenu* NS_MainMenuWidget;

    /** 캐릭터 입력 매핑 컨텍스트 */
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputMappingContext* NS_SGKCharacterInputs;
};