#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/DataTable.h"
#include "NS_GameModeBase.h"
#include "NS_SinglePlayMode.generated.h"

class AActor;
class APawn;
class ANS_PlayerCharacterBase;

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
class TEAMLUNATIC_NOSIGNAL_API ANS_SinglePlayMode : public ANS_GameModeBase
{
	GENERATED_BODY()

public:
	ANS_SinglePlayMode();

	UFUNCTION(BlueprintCallable, Category = "Location")
	virtual FVector GetPlayerLocation_Implementation() const override;
protected:
	virtual void BeginPlay() override;

	// 게임 종료 처리 
	void HandleGameOver(bool bPlayerSurvived, EEscapeRoute EscapeRoute);

private:
	// 게임 상태 
	bool bIsGameOver = false;
	EEscapeRoute CurrentEscapeRoute = EEscapeRoute::None;

	// 참조 캐시 
	UPROPERTY()
	AActor* TrackerZombieInstance;

	UPROPERTY()
	AActor* CachedPlayerStart;

};
