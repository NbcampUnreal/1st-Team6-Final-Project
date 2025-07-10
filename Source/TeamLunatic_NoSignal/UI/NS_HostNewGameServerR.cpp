#include "UI/NS_HostNewGameServerR.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/Spacer.h"
#include "Components/ComboBoxString.h"
#include "Kismet/GameplayStatics.h"
#include "GameFlow/NS_GameInstance.h"

void UNS_HostNewGameServerR::NativeConstruct()
{
    Super::NativeConstruct();
    if (CreateServerButton)
        CreateServerButton->OnClicked.AddDynamic(this, &UNS_HostNewGameServerR::OnCreateServerButtonClicked);
}

FString UNS_HostNewGameServerR::GetSaveSlotName() const
{
    return SaveNameEntryBox ? SaveNameEntryBox->GetText().ToString() : TEXT("DefaultSlot");
}

void UNS_HostNewGameServerR::StartGame()
{
    // 세이브 슬롯명 = 방 이름
    const FString RoomName = GetSaveSlotName();

    // 인원수 (콤보박스 사용시)
    int32 MaxPlayers = 4;
    if (ComboBoxString && ComboBoxString->GetSelectedOption().IsNumeric())
        MaxPlayers = FCString::Atoi(*ComboBoxString->GetSelectedOption());

    // 실제 전달되는 방 이름 로그로 출력!
    UE_LOG(LogTemp, Warning, TEXT("[HostNewGameServerR] CreateSession 호출: 방 이름 = %s, 인원수 = %d"), *RoomName, MaxPlayers);

    if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        GI->SetGameModeType(EGameModeType::MultiPlayMode);
        GI->CreateSession(FName(*RoomName), MaxPlayers);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("GameInstance를 찾을 수 없음"));
    }
}


// 서버생성 버튼 클릭
void UNS_HostNewGameServerR::OnCreateServerButtonClicked()
{
    StartGame();
}
