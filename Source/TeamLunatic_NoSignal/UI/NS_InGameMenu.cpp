// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_InGameMenu.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UI/NS_MasterMenuPanel.h"

void UNS_InGameMenu::NativeConstruct()
{
	Super::NativeConstruct();

	//TArray<UWidget*> AllWidgets;
	//WidgetTree->GetAllWidgets(AllWidgets);

	//for (UWidget* Widget : AllWidgets)
	//{
	//	if (UNS_MasterMenuPanel* Panel = Cast<UNS_MasterMenuPanel>(Widget))
	//	{
	//		EWidgetToggleType ToggleKey = Panel->MyToggleType;

	//		if (ToggleKey == EWidgetToggleType::None)
	//			continue;

	//		//const UEnum* EnumPtr = StaticEnum<EWidgetToggleType>();
	//		//FString EnumName = EnumPtr ? EnumPtr->GetNameStringByValue((int64)ToggleKey) : TEXT("Unknown");
	//		//UE_LOG(LogTemp, Warning, TEXT("PanelName:%s  / ToggleKey: %s"), *Panel->GetName(), *EnumName);

	//		if (!WidgetMap.Contains(ToggleKey))
	//		{
	//			//Panel->MainMenu = this;
	//			WidgetMap.Add(ToggleKey, Panel);
	//		}
	//		else
	//			UE_LOG(LogTemp, Warning, TEXT("Duplicate ToggleType found: %d"), (int32)ToggleKey);
	//	}
	//}
	//for (const TPair<EWidgetToggleType, UNS_MasterMenuPanel*>& Elem : WidgetMap)
	//{
	//	if (Elem.Value)
	//		Elem.Value->Init(this);
	//}
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