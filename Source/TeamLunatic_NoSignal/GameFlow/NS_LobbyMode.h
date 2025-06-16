#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "NS_LobbyMode.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_LobbyMode : public AGameMode
{
    GENERATED_BODY()

public:
    ANS_LobbyMode(); // 생성자 선언
    virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, Category = "Character")
    TArray<TSubclassOf<APawn>> PlayableCharacter;
    void CheckAllPlayersReady();
protected:
    AActor* FindSpawnPointByIndex(int32 Index);


};
