#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/DataTable.h"
#include "NS_GameModeBase.h"
#include "NS_SinglePlayMode.generated.h"

class AActor;
class APawn;
class ANS_PlayerCharacterBase;

// 탈출 루트 구분
UENUM(BlueprintType)
enum class EEscapeRoute : uint8
{
	None,
	Car,
	Radio
};

// 탈출 시도 정보
USTRUCT(BlueprintType)
struct FEscapeAttemptInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	ANS_PlayerCharacterBase* Player = nullptr;

	UPROPERTY(BlueprintReadWrite)
	FName EscapeTargetTag = NAME_None;
};

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_SinglePlayMode : public ANS_GameModeBase
{
	GENERATED_BODY()

public:
	ANS_SinglePlayMode();

	// 플레이어 위치 반환 (예: AI 추격자 사용 목적)
	UFUNCTION(BlueprintCallable, Category = "Location")
	virtual FVector GetPlayerLocation_Implementation() const override;

	// 플레이어 진입 시 처리
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// 사망 이벤트 핸들링
	virtual void OnPlayerCharacterDied_Implementation(ANS_PlayerCharacterBase* DeadCharacter) override;

protected:
	void HandleGameOver(bool bPlayerSurvived, EEscapeRoute EscapeRoute);

private:
	UPROPERTY(EditAnywhere, Category = "Character")
	TArray<TSubclassOf<APawn>> AvailablePawnClasses;

	bool bIsGameOver = false;
	EEscapeRoute CurrentEscapeRoute = EEscapeRoute::None;
};
