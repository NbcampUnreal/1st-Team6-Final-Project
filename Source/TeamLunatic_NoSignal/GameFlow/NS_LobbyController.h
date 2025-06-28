#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NS_LobbyController.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_LobbyController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

public:
	UFUNCTION(Client, Reliable)
	void Client_ShowWait();

	UFUNCTION(Client, Reliable)
	void Client_ShowLoadingScreen();

	// 동기화된 로딩 시스템
	UFUNCTION(Server, Reliable)
	void Server_NotifyLoadingComplete();

	UFUNCTION(Client, Reliable)
	void Client_HideLoadingScreen();

	FTimerHandle CheckLoadingHandle;



};
