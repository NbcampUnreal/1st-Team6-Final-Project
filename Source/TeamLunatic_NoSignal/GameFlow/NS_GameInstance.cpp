// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFlow/NS_GameInstance.h"
#include "Engine/Engine.h"

#include "OnlineSubsystem.h"                          // IOnlineSubsystem
#include "OnlineSessionSettings.h"                    // FOnlineSessionSettings
#include "Interfaces/OnlineSessionInterface.h"        // IOnlineSessionPtr
#include "Kismet/GameplayStatics.h"                   // UGameplayStatics (선택사항)

void UNS_GameInstance::SetGameModeType(EGameModeType Type)
{
	GameModeType = Type;
	UE_LOG(LogTemp, Log, TEXT("[GameInstance] GameModeType set to %s"), *UEnum::GetValueAsString(Type));
}
void UNS_GameInstance::CreateSession(APlayerController* PC, int32 NumConnections, bool bIsLAN)
{
	// OnlineSubsystem을 통한 세션 생성 코드...
	// 성공 시 OnCreateSessionSuccess.Broadcast() 호출
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (!Subsystem) return;

    IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
    if (!SessionInterface.IsValid()) return;

    FName SessionName = NAME_GameSession;

    // 기존 세션 있으면 삭제
    if (SessionInterface->GetNamedSession(SessionName))
    {
        SessionInterface->DestroySession(SessionName);
    }

    // 세션 설정
    SessionSettings = MakeShareable(new FOnlineSessionSettings());
    SessionSettings->bIsLANMatch = bIsLAN;
    SessionSettings->NumPublicConnections = NumConnections;
    SessionSettings->bShouldAdvertise = true;
    SessionSettings->bUsesPresence = true;
    SessionSettings->bAllowJoinInProgress = true;
    SessionSettings->bAllowJoinViaPresence = true;

    // 세션 생성 완료 콜백 바인딩
    SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(
        FOnCreateSessionCompleteDelegate::CreateUObject(this, &UNS_GameInstance::OnCreateSessionComplete)
    );

    // 세션 생성 시도
    SessionInterface->CreateSession(0, SessionName, *SessionSettings);
}

void UNS_GameInstance::SetCurrentSaveSlot(FString SaveSlotName)
{
    CurrentSaveSlotName = SaveSlotName;
}

void UNS_GameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		// 세션 생성 성공
		OnCreateSessionSuccess.Broadcast();
	}
	else
	{
		// 세션 생성 실패 처리
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Failed to create session"));
	}
}
