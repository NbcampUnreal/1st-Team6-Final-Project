#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NS_LobbyController.generated.h"

/**
 * 로비에서 Enter 키를 누르면 서버에 게임 시작 요청을 보내는 컨트롤러
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_LobbyController : public APlayerController
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;

	// 입력 바인딩 초기화
	virtual void SetupInputComponent() override;

	// 클라이언트에서 엔터 키 눌렀을 때 실행
	void HandleStartGame();

	// 서버가 실제 레벨 오픈을 처리
	UFUNCTION(Server, Reliable)
	void Server_RequestStartGame();
};
