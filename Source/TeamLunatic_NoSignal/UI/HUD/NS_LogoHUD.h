#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "NS_LogoHUD.generated.h"


UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_LogoHUD : public AHUD
{
	GENERATED_BODY()

	//로고 UI 위젯
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UUserWidget> LogoWidgetclass;

	//show hide
};
