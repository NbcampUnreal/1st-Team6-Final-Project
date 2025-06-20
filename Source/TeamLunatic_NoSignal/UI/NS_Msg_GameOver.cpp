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
	ExitGameBtn->OnClicked.AddDynamic(this, &UNS_Msg_GameOver::OnQuit);
	ObserverButton->OnClicked.AddDynamic(this, &UNS_Msg_GameOver::OnObserverButtonClicked);
}

void UNS_Msg_GameOver::Init(UNS_BaseMainMenu* NsMainMenu)
{
	Super::Init(NsMainMenu);
}
void UNS_Msg_GameOver::OnQuit()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
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
void UNS_Msg_GameOver::OnLoadSavedGameBtnClicked() //블루프린트에 가보면 버튼은 있으나 안보이게 처리 해놨음. => hidden (기획변경으로 load,save를 없앴으니 숨겨놨음)  
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

void UNS_Msg_GameOver::UpdateBoxVisibility()
{
	UWorld* World = GetWorld();
	if (!World) return;

	UNS_GameInstance* GameInstance = Cast<UNS_GameInstance>(UGameplayStatics::GetGameInstance(World));
	if (!GameInstance) return;

	EGameModeType Mode = GameInstance->GetGameModeType();

	if (ObserverHorizontalBox) 
	{
		ObserverHorizontalBox->SetVisibility(
			Mode == EGameModeType::MultiPlayMode ? ESlateVisibility::Visible : ESlateVisibility::Collapsed
		);
	}
}

void UNS_Msg_GameOver::OnObserverButtonClicked()
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		PC->ChangeState(NAME_Spectating);
		PC->UnPossess();

		if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
		{
			if (UNS_UIManager* UIManager = GI->GetUIManager())
			{
				UIManager->ShowSpectatorWidget(GetWorld());
			}
		}
	}
	this->RemoveFromParent();
}
