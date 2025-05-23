// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFlow/NS_GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

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

	SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UNS_GameInstance::OnCreateSessionComplete);
	SessionInterface->CreateSession(0, SessionName, *SessionSettings);
}

void UNS_GameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Log, TEXT("Session created successfully: %s"), *SessionName.ToString());
		//UGameplayStatics::OpenLevel(GetWorld(), "대기실"); 
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create session: %s"), *SessionName.ToString());
	}
}
