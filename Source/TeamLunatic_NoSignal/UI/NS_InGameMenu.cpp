// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_InGameMenu.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UI/NS_MasterMenuPanel.h"

void UNS_InGameMenu::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 위젯 맵 초기화
	TArray<UWidget*> AllWidgets;
	WidgetTree->GetAllWidgets(AllWidgets);
	
	for (UWidget* Widget : AllWidgets)
	{
		if (UNS_MasterMenuPanel* Panel = Cast<UNS_MasterMenuPanel>(Widget))
		{
			EWidgetToggleType ToggleKey = Panel->GetToggleType();
			
			if (ToggleKey != EWidgetToggleType::None)
			{
				if (!WidgetMap.Contains(ToggleKey))
				{
					WidgetMap.Add(ToggleKey, Panel);
					UE_LOG(LogTemp, Log, TEXT("Added widget to map: %s, ToggleType: %d"), 
						   *Panel->GetName(), (int32)ToggleKey);
				}
				else
					UE_LOG(LogTemp, Warning, TEXT("Duplicate ToggleType found: %d"), (int32)ToggleKey);
			}
		}
	}
	
	// 각 위젯 초기화
	for (const TPair<EWidgetToggleType, UNS_MasterMenuPanel*>& Elem : WidgetMap)
	{
		if (Elem.Value)
			Elem.Value->Init(this);
	}
	
	// 디버그 로그: 위젯 맵 내용 출력
	UE_LOG(LogTemp, Log, TEXT("Widget Map Contents:"));
	for (const TPair<EWidgetToggleType, UNS_MasterMenuPanel*>& Elem : WidgetMap)
	{
		UE_LOG(LogTemp, Log, TEXT("ToggleType: %d, Widget: %s"), 
			   (int32)Elem.Key, Elem.Value ? *Elem.Value->GetName() : TEXT("NULL"));
	}
}
//UNS_MasterMenuPanel* UNS_InGameMenu::GetWidget(EWidgetToggleType ToggleType)
//{
//	if (UNS_MasterMenuPanel* Widget = WidgetMap.FindRef(ToggleType))
//		return Widget;
//	return nullptr;
//}
//
//void UNS_InGameMenu::QuitNSGame()
//{
//	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
//}
void UNS_InGameMenu::PlayAnimationShowR()
{
	PlayAnimation(ShowR);
}

void UNS_InGameMenu::PlayAnimationShowL()
{
	PlayAnimation(ShowL);
}
void UNS_InGameMenu::ShowWidget()
{
	Super::ShowWidget();
	GetWidget(EWidgetToggleType::InGamemStartMenu)->ShowWidget();
}

void UNS_InGameMenu::HideWidget()
{
	Super::HideWidget();
	GetWidget(EWidgetToggleType::InGamemStartMenu)->HideSubMenuWidget();
	GetWidget(EWidgetToggleType::Settings)->HideSubMenuWidget();
	GetWidget(EWidgetToggleType::SaveNameMenu)->HideWidget();
	GetWidget(EWidgetToggleType::AreYouSureMenu)->HideWidget();
	GetWidget(EWidgetToggleType::LoadMenuInGameOver)->HideWidget();
	GetWidget(EWidgetToggleType::PopUpMsg)->HideWidget();
}