#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NS_AnimateWidgetInterface.generated.h"

UINTERFACE()
class UNS_AnimateWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

class TEAMLUNATIC_NOSIGNAL_API INS_AnimateWidgetInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Animation")
	void ShowWidget();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Animation")
	void HideWidget();
};
