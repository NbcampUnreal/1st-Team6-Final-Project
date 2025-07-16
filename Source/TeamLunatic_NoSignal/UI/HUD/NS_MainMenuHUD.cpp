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