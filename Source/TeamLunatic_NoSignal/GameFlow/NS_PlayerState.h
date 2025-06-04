#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NS_PlayerState.generated.h"

/**
 * 로비에서 플레이어 정보를 관리하는 상태 클래스
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_PlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ANS_PlayerState();

	// 레디 여부 (선택적 사용)
	UPROPERTY(ReplicatedUsing = OnRep_IsReady, BlueprintReadOnly, Category = "Lobby")
	bool bIsReady;

	// 레디 상태 설정 (서버에서만 호출)
	void SetIsReady(bool bReady);

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Lobby")
	int32 PlayerIndex;

	void SetPlayerIndex(int32 Index); 
protected:
	UFUNCTION()
	void OnRep_IsReady();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
