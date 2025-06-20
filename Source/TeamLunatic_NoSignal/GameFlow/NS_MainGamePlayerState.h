#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NS_MainGamePlayerState.generated.h"

class APawn;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_MainGamePlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    ANS_MainGamePlayerState();

    // 선택한 캐릭터 클래스 (스폰 시 사용) 
    UPROPERTY(BlueprintReadWrite, Replicated)
    TSubclassOf<APawn> SelectedPawnClass;

    // UI 슬롯 인덱스 
    UPROPERTY(BlueprintReadWrite, Replicated)
    int32 PlayerIndex = -1;

	FORCEINLINE FString GetPlayerModelPath() const { return PlayerModelPath; } // 플레이어 모델 경로 반환
    void LoadPlayerData(); // 플레이어의 데이터를 불러오는 함수

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(BlueprintReadWrite, Replicated)
    bool bIsAlive; // 플레이어의 생존 상태
    
private:
	// 플레이어 모델 경로 (예: 캐릭터 모델 경로)
	UPROPERTY(BlueprintReadWrite, Replicated, Category = "PlayerData", meta = (AllowPrivateAccess = "true"))
	FString PlayerModelPath;
};
