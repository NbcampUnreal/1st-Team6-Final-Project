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

	// 레디 여부 (RepNotify)
	UPROPERTY(ReplicatedUsing = OnRep_IsReady, BlueprintReadOnly, Category = "Lobby")
	bool bIsReady;

	// 서버에서만 호출
	void SetIsReady(bool bReady);

	// 클라 → 서버로 호출하는 RPC
	UFUNCTION(Server, Reliable)
	void ServerSetIsReady(bool bReady);

	// 레디 상태 조회 (UI용)
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	bool GetIsReady() const { return bIsReady; }

	// Index / 모델 관련
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Lobby")
	int32 PlayerIndex;

	void SetPlayerIndex(int32 Index); 
	void SetPlayerModelPath(const FString& ModelPath);
	void SavePlayerData();

protected:
	FString PlayerModelPath;

	// ✅ 상태 변경시 호출
	UFUNCTION()
	void OnRep_IsReady();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
