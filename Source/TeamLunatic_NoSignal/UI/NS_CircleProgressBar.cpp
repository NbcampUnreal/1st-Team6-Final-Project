// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_CircleProgressBar.h"
//#include "Components/Border.h"
#include "Components/Image.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"

void UNS_CircleProgressBar::NativeConstruct()
{
	Super::NativeConstruct();

    CurrentPercent = 1.f;

    // 1) 위젯의 이미지 컴포넌트에서 머티리얼 가져오기
    if (CircleProgressBar)
    {
        UObject* ResourceObject = CircleProgressBar->GetBrush().GetResourceObject();
        UMaterialInterface* Material = Cast<UMaterialInterface>(ResourceObject);
        if (Material)
        {
            DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
            CircleProgressBar->SetBrushFromMaterial(DynamicMaterial);
        }
    }
    if (true)return;

    //GetWorld()->GetTimerManager().SetTimer(
    //    UpdateTimerHandle,
    //    this,
    //    &UNS_CircleProgressBar::UpdatePercent,
    //    0.05f,
    //    true
    //);
    GetWorld()->GetTimerManager().ClearTimer(UpdateTimerHandle);
	TWeakObjectPtr<UNS_CircleProgressBar> SafeThis = this;
    float ElapsedTime = 0.f;
    float PrevTime = GetWorld()->GetTimeSeconds();

	GetWorld()->GetTimerManager().SetTimer
	(
        UpdateTimerHandle,
        FTimerDelegate::CreateLambda([SafeThis, ElapsedTime, PrevTime]() mutable
        {
            if (!SafeThis.IsValid())return;

            UWorld* World = SafeThis->GetWorld();

            if (!World) return;

            SafeThis->CurrentPercent -= (World->GetTimeSeconds() - PrevTime)*0.001f;
            SafeThis->DynamicMaterial->SetScalarParameterValue(FName("Percent"), SafeThis->CurrentPercent);

            //UE_LOG(LogTemp, Warning, TEXT("ElapsedTime %f / idx %d"), ElapsedTime, Index);
            if (SafeThis->CurrentPercent <= 0.f)
            {
                SafeThis->DynamicMaterial->SetScalarParameterValue(FName("Percent"), 0.f);
                World->GetTimerManager().ClearTimer(SafeThis->UpdateTimerHandle);
            }
            else
                PrevTime = World->GetTimeSeconds();
        }), 0.05f, true
	);
}
void UNS_CircleProgressBar::UpdatePercent(float Percent)
{
    if (DynamicMaterial)
        DynamicMaterial->SetScalarParameterValue(FName("Percent"), Percent);
}