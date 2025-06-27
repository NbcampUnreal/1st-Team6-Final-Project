#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NS_LobbyController.generated.h"

/**
 * 로비 환경에서 사용되는 플레이어 컨트롤러 클래스
 * 로비 UI 관리 및 카메라 설정을 담당
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_LobbyController : public APlayerController
{
    GENERATED_BODY()
    
public:
    /** 게임 시작 시 호출 - 로비 환경 초기화 */
    virtual void BeginPlay() override;
    
    /** 폰 소유 시 호출 - 로비 카메라 설정 */
    virtual void OnPossess(APawn* InPawn) override;
    
    /** 
     * 클라이언트에 대기 UI 표시 요청
     * 모든 플레이어가 준비 완료되었을 때 서버에서 호출
     */
    UFUNCTION(Client, Reliable)
    void Client_ShowWait();
};