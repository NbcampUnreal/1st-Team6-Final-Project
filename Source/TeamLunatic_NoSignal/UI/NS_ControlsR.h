// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/NS_MasterMenuPanel.h"
#include "NS_ControlsR.generated.h"

/**
 * 
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_ControlsR : public UNS_MasterMenuPanel
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
};
