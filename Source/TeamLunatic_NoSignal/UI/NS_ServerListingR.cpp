// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_ServerListingR.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "GameFlow/NS_GameInstance.h"

void UNS_ServerListingR::NativeConstruct()
{
    Super::NativeConstruct();

    if (JoinServerButton)
    {
        JoinServerButton->OnClicked.AddDynamic(this, &UNS_ServerListingR::OnJoinButtonClicked);
    }
}

void UNS_ServerListingR::OnJoinButtonClicked()
{
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
    {
        if (!CustomAddress.IsEmpty())
        {
            if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
            {
                GI->ShowWait(); 
            }

            UE_LOG(LogTemp, Log, TEXT("[JoinButton] Connecting to %s"), *CustomAddress);
            PC->ClientTravel(CustomAddress, ETravelType::TRAVEL_Absolute);
        }
    }
}




