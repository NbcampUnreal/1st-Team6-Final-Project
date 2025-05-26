// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFlow/NS_GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "OnlineSubsystem.h"                          // IOnlineSubsystem
#include "OnlineSessionSettings.h"                    // FOnlineSessionSettings
#include "Interfaces/OnlineSessionInterface.h"        // IOnlineSessionPtr
#include <Online/OnlineSessionNames.h>
void UNS_GameInstance::SetGameModeType(EGameModeType Type)
{
	GameModeType = Type;
	UE_LOG(LogTemp, Log, TEXT("[GameInstance] GameModeType set to %s"), *UEnum::GetValueAsString(Type));
}

void UNS_GameInstance::CreateSession(FName SessionName, bool bIsLAN, int32 MaxPlayers)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (!Subsystem) return;

	IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
	if (!SessionInterface.IsValid()) return;

	SessionSettings = MakeShareable(new FOnlineSessionSettings());
	SessionSettings->bIsLANMatch = bIsLAN;
	SessionSettings->NumPublicConnections = MaxPlayers;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bUsesPresence = true;
	SessionSettings->bUseLobbiesIfAvailable = true;
	SessionSettings->BuildUniqueId = 0203;
	SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UNS_GameInstance::OnCreateSessionComplete);
	SessionInterface->CreateSession(0, SessionName, *SessionSettings);

	UE_LOG(LogTemp, Warning, TEXT("[Session] BuildUniqueId = 0x%08X"), SessionSettings->BuildUniqueId);

}

void UNS_GameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Log, TEXT("Session created successfully: %s"), *SessionName.ToString());
		
		if (!WaitingRoom.IsNull())
		{
			FString LevelPath = WaitingRoom.GetLongPackageName();
			UE_LOG(LogTemp, Log, TEXT("Opening level: %s"), *LevelPath);
			UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelPath));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No WaitingRoom assigned. Cannot open level."));
		}

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create session: %s"), *SessionName.ToString());
	}
}

void UNS_GameInstance::FindSessions(bool bIsLAN)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (!Subsystem) return;

	IOnlineSessionPtr Sessions = Subsystem->GetSessionInterface();
	if (!Sessions.IsValid()) return;

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = bIsLAN;
	SessionSearch->MaxSearchResults = 100;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);


	Sessions->OnFindSessionsCompleteDelegates.AddUObject(this, &UNS_GameInstance::OnFindSessionsComplete);
	Sessions->FindSessions(0, SessionSearch.ToSharedRef());
}

void UNS_GameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		IOnlineSessionPtr Sessions = Subsystem->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnFindSessionsCompleteDelegates(this);
		}
	}

	if (bWasSuccessful && SessionSearch.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("FindSessions completed. %d results"), SessionSearch->SearchResults.Num());
		OnSessionSearchSuccess.Broadcast(SessionSearch->SearchResults);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FindSessions failed."));
	}
}

void UNS_GameInstance::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (!Subsystem) return;

	IOnlineSessionPtr Sessions = Subsystem->GetSessionInterface();
	if (!Sessions.IsValid()) return;

	Sessions->OnJoinSessionCompleteDelegates.AddUObject(this, &UNS_GameInstance::OnJoinSessionCompleteInternal);

	Sessions->JoinSession(0, NAME_GameSession, SessionResult);
}

void UNS_GameInstance::OnJoinSessionCompleteInternal(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		IOnlineSessionPtr Sessions = Subsystem->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnJoinSessionCompleteDelegates(this);

			FString ConnectString;
			if (Sessions->GetResolvedConnectString(SessionName, ConnectString))
			{
				UE_LOG(LogTemp, Log, TEXT("Joining session at: %s"), *ConnectString);
				APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
				if (PC)
				{
					PC->ClientTravel(ConnectString, TRAVEL_Absolute);
					OnJoinSessionComplete.Broadcast(true);
					return;
				}
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Failed to join session."));
	OnJoinSessionComplete.Broadcast(false);
}
