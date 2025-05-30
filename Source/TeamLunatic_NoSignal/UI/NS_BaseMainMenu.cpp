// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_BaseMainMenu.h"
#include "UI/NS_MasterMenuPanel.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/KismetSystemLibrary.h"

void UNS_BaseMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	TArray<UWidget*> AllWidgets;
	WidgetTree->GetAllWidgets(AllWidgets);

	for (UWidget* Widget : AllWidgets)
	{
		if (UNS_MasterMenuPanel* Panel = Cast<UNS_MasterMenuPanel>(Widget))
		{
			EWidgetToggleType ToggleKey = Panel->MyToggleType;

			if (ToggleKey == EWidgetToggleType::None)
				continue;

			if (!WidgetMap.Contains(ToggleKey))
			{
				//Panel->MainMenu = this;
				WidgetMap.Add(ToggleKey, Panel);
			}
			else
				UE_LOG(LogTemp, Warning, TEXT("Duplicate ToggleType found: %d"), (int32)ToggleKey);
		}
	}
	for (const TPair<EWidgetToggleType, UNS_MasterMenuPanel*>& Elem : WidgetMap)
	{
		if (Elem.Value)
			Elem.Value->Init(this);
	}
}
void UNS_BaseMainMenu::PlayAnimationShowR()
{
}
void UNS_BaseMainMenu::PlayAnimationShowL()
{
}
UNS_MasterMenuPanel* UNS_BaseMainMenu::GetWidget(EWidgetToggleType ToggleType)
{
	if (UNS_MasterMenuPanel* Widget = WidgetMap.FindRef(ToggleType))
		return Widget;
	return nullptr;
}

void UNS_BaseMainMenu::QuitNSGame()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}