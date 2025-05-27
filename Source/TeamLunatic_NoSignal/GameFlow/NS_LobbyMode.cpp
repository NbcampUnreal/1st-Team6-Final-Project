// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFlow/NS_LobbyMode.h"
#include "NS_LobbyMode.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
//#include "NS_FighterPlayerState.h" 



//void ANS_LobbyMode::PostLogin(APlayerController* NewPlayer)
//{
//	Super::PostLogin(NewPlayer);
//
//	int32 PlayerIndex = GameState->PlayerArray.Num() - 1;
//
//	AActor* StartSpot = FindSpawnPointByIndex(PlayerIndex);
//	if (!StartSpot)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("No PlayerStart found for index %d"), PlayerIndex);
//		return;
//	}
//
//	// 블루프린트에서 설정된 DefaultPawnClass 사용
//	FTransform SpawnTransform = StartSpot->GetActorTransform();
//	APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, SpawnTransform);
//	if (SpawnedPawn)
//	{
//		NewPlayer->Possess(SpawnedPawn);
//	}
//
//	// PlayerState에 인덱스 저장
//	if (APS_FighterPlayerState* PS = Cast<APS_FighterPlayerState>(NewPlayer->PlayerState))
//	{
//		PS->SetPlayerIndex(PlayerIndex);
//	}
//}
