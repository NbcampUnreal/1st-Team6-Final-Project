// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_CompassElement.h"
#include "Components/TextBlock.h"
#include "Components/SizeBox.h"
#include "Misc/DefaultValueHelper.h"

void UNS_CompassElement::NativeConstruct()
{
	Super::NativeConstruct();
}
float UNS_CompassElement::GetSizeBoxWideth()
{
	float Width = TextSizeBox->GetDesiredSize().X;
	return Width;
}

void UNS_CompassElement::SetTextScale(float Scale)
{
	TextSizeBox->SetRenderScale(FVector2D(Scale, Scale));
}
bool UNS_CompassElement::IsNumericString(const FString& Str)
{
	int32 OutInt;
	return FDefaultValueHelper::ParseInt(Str, OutInt);
}
void UNS_CompassElement::SetDirLetter(const FString& DirLetter)
{
	if (IsNumericString(DirLetter))
		bDirLetter = false;
	else
		bDirLetter = true;

	TextDir->SetText(FText::FromString(DirLetter));
	ReturnTextScale();
}
void UNS_CompassElement::ReturnTextScale()
{
	if (bDirLetter)
		TextSizeBox->SetRenderScale(FVector2D(1.1f, 1.1f));
	else
		TextSizeBox->SetRenderScale(FVector2D(0.9f, 0.9f));
}