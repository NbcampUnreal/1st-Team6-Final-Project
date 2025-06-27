#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NS_LobbyMode.generated.h"

/**
 * 게임 로비를 관리하는 게임 모드 클래스
 * 플레이어 접속, 캐릭터 생성, 준비 상태 확인 및 게임 시작을 담당
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_LobbyMode : public AGameModeBase
{
    GENERATED_BODY()
    
public:
    /** 생성자 - 기본 설정 초기화 */
    ANS_LobbyMode();

    /** 게임 시작 시 호출 - 온라인 세션 설정 */
    virtual void BeginPlay() override;
    
    /** 
     * 플레이어 로그인 시 호출 - 캐릭터 생성 및 초기화
     * @param NewPlayer 새로 접속한 플레이어 컨트롤러
     */
    virtual void PostLogin(APlayerController* NewPlayer) override;
    
    /**
     * 플레이어 로그아웃 시 호출 - 플레이어 정리 및 UI 업데이트
     * @param Exiting 로그아웃하는 컨트롤러
     */
    virtual void Logout(AController* Exiting) override;
    
    /**
     * 모든 플레이어의 준비 상태 확인 및 게임 시작 처리
     * 모든 플레이어가 준비 완료되면 게임 레벨로 이동
     */
    void CheckAllPlayersReady();

private:
    /**
     * 인덱스에 해당하는 스폰 포인트 찾기
     * @param Index 플레이어 인덱스
     * @return 스폰 포인트 액터 (없으면 nullptr)
     */
    AActor* FindSpawnPointByIndex(int32 Index);
};