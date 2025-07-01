// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/NS_CommonType.h"
#include "NS_CircleProgressBar.generated.h"


UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_CircleProgressBar : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void UpdatePercent(float Percent);

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UImage* Imageback;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UImage* CircleProgressBar;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UImage* ImageSymbol;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= "EStatusType")
	EStatusType StatusType;
private:
	UMaterialInstanceDynamic* DynamicMaterial;
	float CurrentPercent;
	FTimerHandle UpdateTimerHandle;
};
