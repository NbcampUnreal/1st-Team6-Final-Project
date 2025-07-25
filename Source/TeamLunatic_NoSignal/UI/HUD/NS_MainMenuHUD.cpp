#include "NS_MainMenuHUD.h"
#include "Blueprint/UserWidget.h"
#include "UI/MainMenu/NS_AnimateWidgetInterface.h"

void ANS_MainMenuHUD::ShowMainMenuUI()
{
	if (!MainMenuWidget)
	{
		if (MainMenuWidgetClass)
		{
			MainMenuWidget = CreateWidget<UUserWidget>(GetWorld(), MainMenuWidgetClass);
			if (MainMenuWidget)
			{
				MainMenuWidget->AddToViewport();
			}
		}
	}

	if (MainMenuWidget)
	{
		if (MainMenuWidget->GetVisibility() == ESlateVisibility::Collapsed)
		{
			MainMenuWidget->SetVisibility(ESlateVisibility::Visible);
			if (MainMenuWidget->GetClass()->ImplementsInterface(UNS_AnimateWidgetInterface::StaticClass()))
			{
				INS_AnimateWidgetInterface::Execute_ShowWidget(MainMenuWidget);
			}
		}
	}
}

//메인 메뉴가 숨겨지는건, 이미 버튼을 눌러서 애니메이션이 실행되었을때이다.
//어차피 버튼을 눌러서 숨겨지는건 여전하니까, 그냥 위젯이 숨김되게 처리.
void ANS_MainMenuHUD::HideMainMenuUI()
{
	if (MainMenuWidget)
	{
		MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void ANS_MainMenuHUD::ShowSingleUI()
{
	if (!SinglePlayWidget)
	{
		if (SinglePlayWidgetclass)
		{
			SinglePlayWidget = CreateWidget<UUserWidget>(GetWorld(), SinglePlayWidgetclass);
			if (SinglePlayWidget)
			{
				SinglePlayWidget->AddToViewport();
			}
		}
	}

	if (SinglePlayWidget)
	{
		if (SinglePlayWidget->GetVisibility() == ESlateVisibility::Collapsed)
		{
			SinglePlayWidget->SetVisibility(ESlateVisibility::Visible);
			if (SinglePlayWidget->GetClass()->ImplementsInterface(UNS_AnimateWidgetInterface::StaticClass()))
			{
				INS_AnimateWidgetInterface::Execute_ShowWidget(SinglePlayWidget);
			}
		}
	}
}

void ANS_MainMenuHUD::HideSingleUI()
{
	if (SinglePlayWidget)
	{
		SinglePlayWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void ANS_MainMenuHUD::ShowMultiUI()
{
	if (!MultiPlayWidget)
	{
		if (MultiPlayWidgetclass)
		{
			MultiPlayWidget = CreateWidget<UUserWidget>(GetWorld(), MultiPlayWidgetclass);
			if (MultiPlayWidget)
			{
				MultiPlayWidget->AddToViewport();
			}
		}
	}
	
	if (MultiPlayWidget)
	{
		if (MultiPlayWidget->GetVisibility() == ESlateVisibility::Collapsed)
		{
			MultiPlayWidget->SetVisibility(ESlateVisibility::Visible);
			if (MultiPlayWidget->GetClass()->ImplementsInterface(UNS_AnimateWidgetInterface::StaticClass()))
			{
				INS_AnimateWidgetInterface::Execute_ShowWidget(MultiPlayWidget);
			}
		}
	}
}

void ANS_MainMenuHUD::HideMultiUI()
{
	if (MultiPlayWidget)
	{
		MultiPlayWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void ANS_MainMenuHUD::ShowSettingUI()
{
	if (!SettingWidget)
	{
		if (SettingWidgetclass)
		{
			SettingWidget = CreateWidget<UUserWidget>(GetWorld(), SettingWidgetclass);
			if (SettingWidget)
			{
				SettingWidget->AddToViewport();
			}
		}
	}

	if (SettingWidget)
	{
		if (SettingWidget->GetVisibility() == ESlateVisibility::Collapsed)
		{
			SettingWidget->SetVisibility(ESlateVisibility::Visible);
			if (SettingWidget->GetClass()->ImplementsInterface(UNS_AnimateWidgetInterface::StaticClass()))
			{
				INS_AnimateWidgetInterface::Execute_ShowWidget(SettingWidget);
			}
		}
	}
}

void ANS_MainMenuHUD::HideSettingUI()
{
	if (SettingWidget)
	{
		SettingWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}
