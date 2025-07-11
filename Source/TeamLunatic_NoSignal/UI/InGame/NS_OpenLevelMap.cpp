#include "NS_OpenLevelMap.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "TeamLunatic_NoSignal/Character/NS_PlayerCharacterBase.h"

/**
 * @brief 플레이어의 위치를 지도 위에서 업데이트하는 함수입니다.
 * @param NormalizedPosition 정규화된(0.0 ~ 1.0) 플레이어의 지도 내 위치입니다.
 *        이 값은 지도 이미지의 크기에 비례하여 실제 UI 좌표로 변환됩니다.
 *        블루프린트에서 호출 가능합니다.
 */
void UNS_OpenLevelMap::UpdatePlayerLocation(FVector2D NormalizedPosition)
{
    // PlayerLocationIcon(플레이어 위치 아이콘)이 유효한지 확인합니다.
    if (PlayerLocationIcon)
    {
        // 정규화된 위치를 0.0 ~ 1.0 범위로 클램프(제한)하여 유효한 범위 내에 있도록 합니다.
        NormalizedPosition.X = FMath::Clamp(NormalizedPosition.X, 0.0f, 1.0f);
        NormalizedPosition.Y = FMath::Clamp(NormalizedPosition.Y, 0.0f, 1.0f);
        
        // 맵 위젯의 실제 렌더링 크기를 가져옵니다.
        // 이는 디자이너에서 설정된 고정된 사이즈가 아닌, 현재 화면에 렌더링되고 있는 실제 크기입니다.
        const FVector2D MapWidgetRenderSize = GetCachedGeometry().GetLocalSize();
        
        // 아이콘의 원하는 크기를 정의합니다. (블루프린트에서 설정 가능하도록 UPROPERTY로 뺄 수도 있습니다)
        const FVector2D IconSize(20.0f, 20.0f); // 원하는 아이콘 크기로 조정하세요.
        
        // 아이콘의 픽셀 좌표를 계산합니다.
        // 정규화된 위치에 맵 위젯의 렌더링 크기를 곱하여 절대 픽셀 위치를 얻고,
        // 아이콘이 중앙에 오도록 아이콘 크기의 절반을 뺍니다.
        const FVector2D IconPixelPosition = (NormalizedPosition * MapWidgetRenderSize) - (IconSize * 0.5f);
        
        // 플레이어 위치 아이콘의 원하는 크기를 설정합니다.
        PlayerLocationIcon->SetDesiredSizeOverride(IconSize);
        
        // 플레이어 위치 아이콘의 렌더링 위치를 설정합니다.
        // RenderTranslation은 패널 내에서의 상대적인 위치를 설정합니다.
        PlayerLocationIcon->SetRenderTranslation(IconPixelPosition);
        
        // 플레이어 위치 아이콘이 항상 보이도록 설정합니다.
        PlayerLocationIcon->SetVisibility(ESlateVisibility::Visible);
    }
}

/**
 * @brief 위젯이 생성될 때 호출되는 초기화 함수입니다.
 *        주로 위젯의 초기 상태 설정 및 바인딩에 사용됩니다.
 */
void UNS_OpenLevelMap::NativeConstruct()
{
    Super::NativeConstruct();
    
    // LevelMinBound와 LevelMaxBound가 초기화되지 않았다면 기본값을 설정합니다.
    // 이 값들은 월드 좌표계에서 지도의 경계를 나타냅니다.
    if (LevelMinBound.IsZero() && LevelMaxBound.IsZero())
    {
        LevelMinBound = FVector2D(-37500.0f, -37500.0f);
        LevelMaxBound = FVector2D(37500.0f, 37500.0f);
    }

    // PlayerLocationIcon이 블루프린트에서 제대로 바인딩되었는지 확인하는 로그를 출력합니다.
    if (!PlayerLocationIcon)
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerLocationIcon is not bound in UNS_OpenLevelMap widget!"));
    }
}

/**
 * @brief 매 프레임마다 호출되는 틱 함수입니다.
 * @param MyGeometry 위젯의 지오메트리 정보입니다.
 * @param InDeltaTime 마지막 프레임 이후 경과된 시간입니다.
 *        주로 플레이어 위치를 지속적으로 갱신하는 데 사용됩니다.
 */
void UNS_OpenLevelMap::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    // 플레이어 컨트롤러를 가져옵니다.
    APlayerController* PC = GetOwningPlayer();
    if (!PC) return;

    // 플레이어 캐릭터를 가져옵니다.
    ANS_PlayerCharacterBase* PlayerCharacter = Cast<ANS_PlayerCharacterBase>(PC->GetPawn());
    if (!PlayerCharacter) return;

    // 현재 캐릭터의 월드 위치를 가져옵니다.
    FVector WorldLocation = PlayerCharacter->GetActorLocation();

    // 월드 좌표를 0.0 ~ 1.0 범위의 정규화된 맵 좌표로 변환합니다.
    // 언리얼 엔진의 좌표계와 맵 이미지의 방향에 따라 X, Y 매핑이 달라질 수 있습니다.
    // 현재는 맵의 X축이 월드의 Y축에, 맵의 Y축이 월드의 X축에 매핑된다고 가정합니다.
    
    // X축 정규화 비율 계산: (현재 월드 Y 좌표 - 최소 월드 Y 경계) / (최대 월드 Y 경계 - 최소 월드 Y 경계)
    float MapXRatio = (WorldLocation.Y - LevelMinBound.Y) / (LevelMaxBound.Y - LevelMinBound.Y);
    // Y축 정규화 비율 계산: (현재 월드 X 좌표 - 최소 월드 X 경계) / (최대 월드 X 경계 - 최소 월드 X 경계)
    float MapYRatio = (WorldLocation.X - LevelMinBound.X) / (LevelMaxBound.X - LevelMinBound.X);

    // Y축 뒤집기: UI 좌표계와 월드 좌표계의 Y축 방향이 다를 경우 필요합니다.
    // 맵 이미지의 상단이 월드의 +Y 방향이고 하단이 -Y 방향이며, UI의 Y축이 상단이 0, 하단이 1이라면 이 로직이 맞습니다.
    MapYRatio = 1.0f - MapYRatio; // 현재 코드 유지

    // 계산된 정규화된 플레이어 위치를 사용하여 아이콘을 업데이트합니다.
    UpdatePlayerLocation(FVector2D(MapXRatio, MapYRatio));
}