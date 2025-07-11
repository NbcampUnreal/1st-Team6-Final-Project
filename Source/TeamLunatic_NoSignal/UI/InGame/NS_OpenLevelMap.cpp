#include "NS_OpenLevelMap.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "TeamLunatic_NoSignal/Character/NS_PlayerCharacterBase.h"

void UNS_OpenLevelMap::UpdatePlayerLocation(FVector2D NormalizedPosition)
{
    if (PlayerLocationIcon)
    {
        // 정규화된 위치를 0-1 범위로 클램프
        NormalizedPosition.X = FMath::Clamp(NormalizedPosition.X, 0.0f, 1.0f);
        NormalizedPosition.Y = FMath::Clamp(NormalizedPosition.Y, 0.0f, 1.0f);
        
        // 맵 위젯의 실제 렌더링 크기 가져오기
        // 이는 Designer에서 설정된 사이즈가 아닌, 현재 화면에 렌더링되고 있는 실제 크기입니다.
        const FVector2D MapWidgetRenderSize = GetCachedGeometry().GetLocalSize();
        
        // 아이콘 크기 (Blueprint에서 설정 가능하도록 UPROPERTY로 뺄 수도 있습니다)
        const FVector2D IconSize(20.0f, 20.0f); // 원하는 아이콘 크기로 조정하세요.
        
        // 아이콘 위치 계산: 정규화된 위치에 맵 위젯 크기를 곱하여 픽셀 좌표를 얻습니다.
        // 그리고 아이콘이 중앙에 오도록 아이콘 크기의 절반을 뺍니다.
        const FVector2D IconPixelPosition = (NormalizedPosition * MapWidgetRenderSize) - (IconSize * 0.5f);
        
        // 아이콘 크기 설정
        PlayerLocationIcon->SetDesiredSizeOverride(IconSize);
        
        // 아이콘 위치 설정 (렌더 트랜슬레이션은 패널 내에서의 상대적인 위치를 설정합니다)
        PlayerLocationIcon->SetRenderTranslation(IconPixelPosition);
        
        // 아이콘이 항상 보이도록 설정
        PlayerLocationIcon->SetVisibility(ESlateVisibility::Visible);
    }
}

void UNS_OpenLevelMap::NativeConstruct()
{
    Super::NativeConstruct();
    
    if (LevelMinBound.IsZero() && LevelMaxBound.IsZero())
    {
        LevelMinBound = FVector2D(-37500.0f, -37500.0f);
        LevelMaxBound = FVector2D(37500.0f, 37500.0f);
    }

    // PlayerLocationIcon이 유효한지 확인하는 것이 좋습니다.
    if (!PlayerLocationIcon)
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerLocationIcon is not bound in UNS_OpenLevelMap widget!"));
    }
}

void UNS_OpenLevelMap::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    // 플레이어 컨트롤러 가져오기
    APlayerController* PC = GetOwningPlayer();
    if (!PC) return;

    // 플레이어 캐릭터 가져오기
    ANS_PlayerCharacterBase* PlayerCharacter = Cast<ANS_PlayerCharacterBase>(PC->GetPawn());
    if (!PlayerCharacter) return;

    // 현재 캐릭터 월드 위치 가져오기
    FVector WorldLocation = PlayerCharacter->GetActorLocation();

    // 월드 좌표를 0.0 ~ 1.0 범위의 정규화된 맵 좌표로 변환
    // 맵의 X축이 월드의 Y축에, 맵의 Y축이 월드의 X축에 매핑된다고 가정합니다.
    // 이는 언리얼 엔진의 좌표계와 맵 이미지의 방향에 따라 달라질 수 있으므로,
    // 실제 게임 내에서 테스트하면서 XRatio와 YRatio의 매핑을 바꿔보거나
    // (WorldLocation.Y - LevelMinBound.Y) / (LevelMaxBound.Y - LevelMinBound.Y)
    // (WorldLocation.X - LevelMinBound.X) / (LevelMaxBound.X - LevelMinBound.X)
    // 이 부분의 X, Y를 바꿔보면서 올바른 방향을 찾아야 합니다.
    
    float MapXRatio = (WorldLocation.Y - LevelMinBound.Y) / (LevelMaxBound.Y - LevelMinBound.Y);
    float MapYRatio = (WorldLocation.X - LevelMinBound.X) / (LevelMaxBound.X - LevelMinBound.X);

    // Y축 뒤집기 (UI 좌표계와 월드 좌표계의 Y축 방향이 다를 경우 필요)
    // 이 부분이 올바른지 확인해야 합니다. 맵 이미지가 어떤 방향으로 되어있는지에 따라 달라집니다.
    // 예를 들어, 맵 이미지의 상단이 월드의 +Y 방향이고 하단이 -Y 방향이라면,
    // 그리고 UI의 Y축이 상단이 0, 하단이 1이라면 이 뒤집기 로직이 맞을 수 있습니다.
    MapYRatio = 1.0f - MapYRatio; // 현재 코드 유지

    // 정규화된 플레이어 위치를 사용하여 아이콘 업데이트
    UpdatePlayerLocation(FVector2D(MapXRatio, MapYRatio));
}