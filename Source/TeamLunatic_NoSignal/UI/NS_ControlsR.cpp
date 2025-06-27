// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_ControlsR.h"

void UNS_ControlsR::NativeConstruct()
{
	Super::NativeConstruct();
	// Initialize any specific controls here if needed
}

EWidgetToggleType UNS_ControlsR::GetToggleType() const
{
	return EWidgetToggleType::Controls;
}
