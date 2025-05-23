// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_MainMenu.h"
#include "UI/NS_BaseWidgetInterface.h"
#include "UI/NS_SinglePlayWidget.h"
#include "UI/NS_TheSettingsWidget.h"
#include "UI/NS_MasterMenuPanel.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/KismetSystemLibrary.h"

void UNS_MainMenu::NativeConstruct()
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

			const UEnum* EnumPtr = StaticEnum<EWidgetToggleType>();
			FString EnumName = EnumPtr ? EnumPtr->GetNameStringByValue((int64)ToggleKey) : TEXT("Unknown");
			UE_LOG(LogTemp, Warning, TEXT("PanelName:%s  / ToggleKey: %s"), *Panel->GetName(), *EnumName);

			if (ToggleKey == EWidgetToggleType::SinglePlayer)
			{
				int32 dsaf = 0;
				UE_LOG(LogTemp, Warning, TEXT("UNS_MainMenu NativeConstruct"));
			}
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

void UNS_MainMenu::PlayAnimationShowR()
{
	PlayAnimation(ShowR);
}

void UNS_MainMenu::PlayAnimationShowL()
{
	PlayAnimation(ShowL);
}

void  UNS_MainMenu::SelectWidget(EWidgetToggleType ToggleType)
{
	if (ToggleType == EWidgetToggleType::Quit)
	{
		QuitNSGame();
	}
	else
	{
		if (UNS_MasterMenuPanel* Widget = WidgetMap.FindRef(ToggleType))
			Widget->ShowWidget();
		else
		{
			const UEnum* EnumPtr = StaticEnum<EWidgetToggleType>();
			FString EnumName = EnumPtr ? EnumPtr->GetNameStringByValue((int64)ToggleType) : TEXT("Unknown");

			UE_LOG(LogTemp, Warning, TEXT("ERROR!! TMap SelectWidget!! %s"), *EnumName);
		}
	}
}

UNS_MasterMenuPanel* UNS_MainMenu::GetWidget(EWidgetToggleType ToggleType)
{
	if (UNS_MasterMenuPanel* Widget = WidgetMap.FindRef(ToggleType))
		return Widget;
	return nullptr;
}
void UNS_MainMenu::QuitNSGame()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}
