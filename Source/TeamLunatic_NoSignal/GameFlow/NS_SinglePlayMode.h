#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/DataTable.h"
#include "NS_SinglePlayMode.generated.h"

class AActor;
class APawn;
class ANS_PlayerCharacterBase;
//class AZombieSpawnManager;  //좀비스폰매니저
//class AItemSpawnManager;    // 아이템스폰 매니저 

/** 탈출 루트 구분 */
UENUM(BlueprintType)
enum class EEscapeRoute : uint8
{
	None,
	Car,
	Radio
};

/** 탈출 시도 정보 */
USTRUCT(BlueprintType)
struct FEscapeAttemptInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	ANS_PlayerCharacterBase* Player = nullptr;

	UPROPERTY(BlueprintReadWrite)
	FName EscapeTargetTag = NAME_None; // "Car" or "Radio"
};

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_SinglePlayMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ANS_SinglePlayMode();

protected:
	virtual void BeginPlay() override;

	/** 플레이어 스폰 */
	void SpawnPlayer();

	/** 추적자 좀비 스폰 (1마리만) */
	void SpawnTrackerZombie();

	///** 일반 좀비 스폰 (매니저 요청) */
	//void SpawnGeneralZombies();

	///** 아이템 랜덤 배치 (매니저 요청) */
	//void SpawnEscapeItems();
	//void SpawnHealingItems();

	/** 게임 종료 처리 */
	void HandleGameOver(bool bPlayerSurvived, EEscapeRoute EscapeRoute);

	/** 엔딩 조건 확인 */
	bool CheckCarEscapeCondition(const TArray<FName>& PlayerItems) const;
	bool CheckRadioEscapeCondition(const TArray<FName>& PlayerItems) const;

	/** 좀비에게 소리 위치 브로드캐스트 */
	void BroadcastNoiseLocation(const FVector& Location);

public:
	/** 함정 등 외부에서 소리 전달 */
	UFUNCTION(BlueprintCallable, Category = "Sound")
	void RegisterLoudNoise(FVector SoundOrigin);

	/** 엔딩 상호작용 시도 */
	UFUNCTION(BlueprintCallable, Category = "Ending")
	void TryEscape(const FEscapeAttemptInfo& Info);

private:
	/** 게임 상태 */
	bool bIsGameOver = false;
	EEscapeRoute CurrentEscapeRoute = EEscapeRoute::None;

	/** 설정용 클래스들 */
	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<APawn> PlayerPawnClass;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<AActor> TrackerZombieClass;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<AActor> HealingItemClass;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	UDataTable* EscapeItemTable;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	int32 HealingItemCount = 10;

	UPROPERTY(EditAnywhere, Category = "Sound")
	float ZombieReactionRange = 1500.0f;

	/** 참조 캐시 */
	UPROPERTY()
	AActor* TrackerZombieInstance;

	UPROPERTY()
	AActor* CachedPlayerStart;

	/*UPROPERTY()
	AZombieSpawnManager* ZombieSpawnManager;

	UPROPERTY()
	AItemSpawnManager* ItemSpawnManager;*/
};
