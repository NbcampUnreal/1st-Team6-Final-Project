// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_Msg_GameOver.h"
#include "UI/NS_BaseMainMenu.h"
#include "UI/NS_MasterMenuPanel.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"  
#include "GameFlow/NS_GameInstance.h"
#include "UI/NS_UIManager.h"

void UNS_Msg_GameOver::NativeConstruct()
{
	Super::NativeConstruct();
	LoadSavedGameBtn->OnClicked.AddDynamic(this, &UNS_Msg_GameOver::OnLoadSavedGameBtnClicked);
	MainMenuBtn->OnClicked.AddDynamic(this, &UNS_Msg_GameOver::OnMainMenuBtnClicked);
}

void UNS_Msg_GameOver::Init(UNS_BaseMainMenu* NsMainMenu)
{
	Super::Init(NsMainMenu);
}

void UNS_Msg_GameOver::ShowWidgetD()
{
	Super::ShowWidgetD();
	if (UNS_GameInstance* NS_GameInstance = Cast<UNS_GameInstance>(GetGameInstance()))
	{
		if (NS_GameInstance->bIsSinglePlayer)
			LoadSavedGameBtn->SetVisibility(ESlateVisibility::Visible);
		else
			LoadSavedGameBtn->SetVisibility(ESlateVisibility::Collapsed);
	}
	PlayOpenAnimation();
}

void UNS_Msg_GameOver::PlayOpenAnimation()
{
	if (FadeIn)
	{
		PlayAnimation(FadeIn);
	}
}
void UNS_Msg_GameOver::OnLoadSavedGameBtnClicked()
{
	//HideWidget();
	if (UNS_GameInstance* NS_GameInstance = Cast<UNS_GameInstance>(GetGameInstance()))
	{
		if (UNS_UIManager* UIManager = NS_GameInstance->GetUIManager())
		{
			if (UIManager->ShowInGameMenuWidget(GetWorld()))
			{
				UIManager->HideGameOverWidget(GetWorld());
				UIManager->ShowLoadGameWidget(GetWorld());
			}
		}
	}
}
void UNS_Msg_GameOver::OnMainMenuBtnClicked()
{
	//HideWidget();
	if (UNS_GameInstance* NS_GameInstance = Cast<UNS_GameInstance>(GetGameInstance()))
	{
		if (UNS_UIManager* UIManager = NS_GameInstance->GetUIManager())
			UIManager->HideGameOverWidget(GetWorld());
	}
	UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("MainTitle")));//MenuMap / MainTitle
}