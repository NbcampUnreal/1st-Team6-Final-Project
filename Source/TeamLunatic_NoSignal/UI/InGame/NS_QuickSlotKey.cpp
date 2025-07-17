#include "UI/InGame/NS_QuickSlotKey.h"
#include "Components/TextBlock.h"

/**
 * @brief 이 퀵 슬롯 키 위젯이 나타낼 슬롯의 인덱스를 설정합니다.
 * @param Index 설정할 슬롯의 인덱스입니다. 이 인덱스는 화면에 표시될 키 번호로 사용됩니다.
 *        블루프린트에서 호출 가능합니다.
 */
void UNS_QuickSlotKey::SetSlotIndex(int32 Index)
{
    // QSText(텍스트 블록)가 유효하면 슬롯 인덱스를 텍스트로 변환하여 설정합니다.
    // 인덱스는 0부터 시작하지만, 사용자에게는 1부터 시작하는 번호로 보여주기 위해 Index + 1을 합니다.
    if (QSText)
    {
        QSText->SetText(FText::FromString(FString::FromInt(Index + 1))); // 예: 0번 인덱스는 "1"로 표시
    }
}