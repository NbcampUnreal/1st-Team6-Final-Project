// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFlow/NS_GameInstance.h"
#include "Engine/Engine.h"

void UNS_GameInstance::SetGameModeType(EGameModeType Type)
{
	GameModeType = Type;
	UE_LOG(LogTemp, Log, TEXT("[GameInstance] GameModeType set to %s"), *UEnum::GetValueAsString(Type));
}
