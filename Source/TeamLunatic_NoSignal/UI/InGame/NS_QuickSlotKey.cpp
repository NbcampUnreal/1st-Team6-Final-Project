#include "UI/InGame/NS_QuickSlotKey.h"
#include "Components/TextBlock.h"

void UNS_QuickSlotKey::SetSlotIndex(int32 Index)
{
    if (QSText)
    {
        QSText->SetText(FText::FromString(FString::FromInt(Index + 1))); // 0 → "1"
    }
}