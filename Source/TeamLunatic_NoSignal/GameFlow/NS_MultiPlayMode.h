#pragma once
#include "CoreMinimal.h"
#include "NS_GameModeBase.h"
#include "NS_MultiPlayMode.generated.h"

class ANS_PlayerCharacterBase;
class APawn;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_MultiPlayMode : public ANS_GameModeBase
{
    GENERATED_BODY()

public:
    ANS_MultiPlayMode();
    virtual FVector GetPlayerLocation_Implementation() const override;
    virtual void OnPlayerCharacterDied_Implementation(ANS_PlayerCharacterBase* DeadCharacter) override;
    void PostLogin(APlayerController* NewPlayer);
    virtual void Logout(AController* Exiting) override;  // 플레이어 로그아웃 시 호출


    // 최적화된 스폰 체크 함수
    UFUNCTION()
    void OptimizedMultiplaySpawnCheck();

    UPROPERTY(BlueprintReadWrite, Category = "Game State")
    bool bIsGameOver = false;
    
    // 로딩 동기화 시스템
    UPROPERTY()
    TArray<APlayerController*> LoadingCompletedPlayers;

    UFUNCTION()
    void OnPlayerLoadingComplete(APlayerController* Player);

    UFUNCTION()
    void CheckAllPlayersLoadingComplete();

protected:
    virtual void BeginPlay() override;
    FVector GetRandomPlayerLocation() const;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
    TArray<TSubclassOf<APawn>> MainGamePawnClassesToSpawn;

    UPROPERTY()
    TArray<TSubclassOf<APawn>> AvailablePawnsToSpawn;

private:
    FTimerHandle PlayerCountCheckTimer;
    void CheckPlayerCountAndEndSession();

    // Flask 서버에 플레이어 로그인/로그아웃 알림 함수들
    void NotifyPlayerLogin();
    void NotifyPlayerLogout();

    // 캐릭터 중복 방지 시스템
    UPROPERTY()
    TArray<int32> UsedPawnIndices; // 이미 사용된 폰 인덱스들

    // 중복되지 않는 폰 클래스 선택 함수
    TSubclassOf<APawn> GetUniqueRandomPawnClass();

    // 플레이어 로그아웃 시 사용된 폰 인덱스 해제
    void ReleasePawnIndex(APlayerController* ExitingPlayer);

    // 캐릭터 중복 방지 시스템 리셋 (게임 재시작 시 사용)
    UFUNCTION(BlueprintCallable, Category = "Character Management")
    void ResetCharacterDuplicationSystem();
};