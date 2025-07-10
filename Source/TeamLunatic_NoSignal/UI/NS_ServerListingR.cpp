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
    // 반드시 FOnlineSessionSearchResult 기반 Join
    if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        GI->JoinSession(SessionResult);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[NS_ServerListingR] GameInstance 캐스팅 실패!"));
    }
}
