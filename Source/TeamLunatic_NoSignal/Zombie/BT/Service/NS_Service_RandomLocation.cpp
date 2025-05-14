// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie/BT/Service/NS_Service_RandomLocation.h"
#include "NavigationSystem.h"
#include "GameFramework/Pawn.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UNS_Service_RandomLocation::UNS_Service_RandomLocation()
{
	bNotifyTick = true;
	bNotifyBecomeRelevant = true;
	Interval = 2.0f;
	Radius = 150.f;
	NodeName = "Generate Random Location";
}

void UNS_Service_RandomLocation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* AIPawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!AIPawn) return;

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSys) return;

	FVector Origin = AIPawn->GetActorLocation();
	FNavLocation RandomLocation;

	if (NavSys->GetRandomReachablePointInRadius(Origin, Radius, RandomLocation))
	{
		GEngine->AddOnScreenDebugMessage(
		-1,                      // Key (-1이면 중복 허용)
		2.0f,                    // 지속 시간 (초)
		FColor::Green,           // 텍스트 색상
		TEXT("서비스가 호출되었습니다!") // 출력 내용
		);

		OwnerComp.GetBlackboardComponent()->SetValueAsVector(RandomLocationKey.SelectedKeyName,RandomLocation.Location);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow,
				FString::Printf(TEXT("랜덤 위치 키: %s"), *RandomLocationKey.SelectedKeyName.ToString()));
		}
	}
}



