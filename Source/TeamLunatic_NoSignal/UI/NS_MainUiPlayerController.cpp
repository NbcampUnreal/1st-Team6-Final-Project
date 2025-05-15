// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_MainUiPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
//#include "SGKGameInstance.h"
#include "TempGameInstance.h"
#include "NS_UIManager.h"
#include "NS_MainMenu.h"

void ANS_MainUiPlayerController::BeginPlay()
{
	Super::BeginPlay();

    if (!IsLocalController())
        return;

    if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (NS_SGKCharacterInputs)
            {
                InputSubsystem->AddMappingContext(NS_SGKCharacterInputs, 0);
            }
        }
    }

    //if (USGKGameInstance* SGKInstance = Cast<USGKGameInstance>(GetGameInstance()))
    //{
    //    SGKInstance->LoadControls();
    //}
    //if (NS_MainMenuWidgetClass)
    //{
    //    NS_MainMenuWidget = CreateWidget(this, NS_MainMenuWidgetClass);
    //    if (NS_MainMenuWidget)
    //        NS_MainMenuWidget->AddToViewport();
    //}
     UNS_MainMenu* MainMenuWidget = nullptr;
     if (UTempGameInstance* TempGameInstance = Cast<UTempGameInstance>(GetGameInstance()))
     {
         TempGameInstance->GetUIManager()->ShowMainMenuWidget(GetWorld());
         MainMenuWidget = TempGameInstance->GetUIManager()->GetNS_MainMenuWidget();
     }

    // 4. 마우스 커서 표시 및 입력 모드 설정
    bShowMouseCursor = true;

    FInputModeUIOnly InputMode;
    InputMode.SetWidgetToFocus(MainMenuWidget->TakeWidget());
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(InputMode);

}
