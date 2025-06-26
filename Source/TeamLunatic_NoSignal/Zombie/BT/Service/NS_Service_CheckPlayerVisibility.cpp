#include "NS_Service_CheckPlayerVisibility.h" 
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"

UNS_Service_CheckPlayerVisibility::UNS_Service_CheckPlayerVisibility()
{
	NodeName = TEXT("C++ Check Player Visibility"); // BT 에디터에서 보일 이름
	Interval = 0.15f;        // 0.15초마다 한 번씩 체크
	RandomDeviation = 0.05f; // 실행 주기에 약간의 무작위성을 부여
}

void UNS_Service_CheckPlayerVisibility::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// ===================== 1번 로그: 서비스 실행 여부 확인 =====================
	UE_LOG(LogTemp, Warning, TEXT("--- C++ 서비스 TickNode 실행됨 ---"));

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!IsValid(BlackboardComp)) return;

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!IsValid(AIController)) return;

	APawn* AIPawn = AIController->GetPawn();
	if (!IsValid(AIPawn)) return;

	ACharacter* TargetPlayer = Cast<ACharacter>(BlackboardComp->GetValueAsObject(TargetPlayerKey.SelectedKeyName));
	if (!IsValid(TargetPlayer))
	{
		// 이 로그를 통해 TargetPlayer를 가져오지 못하는 문제를 확인할 수 있습니다.
		UE_LOG(LogTemp, Error, TEXT("서비스 오류: 블랙보드에서 TargetPlayer를 가져올 수 없습니다!"));
		BlackboardComp->SetValueAsBool(IsPlayerLookingKey.SelectedKeyName, false);
		return;
	}

	bool bIsCurrentlyVisible = false;

	const FVector AILocation = AIPawn->GetActorLocation();
	const FVector PlayerForward = TargetPlayer->GetActorForwardVector();
	const FVector DirectionToAI = (AILocation - TargetPlayer->GetActorLocation()).GetSafeNormal();

	const float DotResult = FVector::DotProduct(PlayerForward, DirectionToAI);

	// ===================== 2번 로그: Dot Product 결과값 확인 =====================
	UE_LOG(LogTemp, Warning, TEXT("Dot Product 결과: %f"), DotResult);

	if (DotResult > 0.0f)
	{
		// ===================== 3번 로그: 조건문 통과 여부 확인 =====================
		UE_LOG(LogTemp, Log, TEXT("Dot Product 통과, 라인 트레이스 실행 직전."));

		FHitResult HitResult;
		FVector TraceStart;
		FRotator ViewRotation;
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsValid(PlayerController))
		{
			PlayerController->GetPlayerViewPoint(TraceStart, ViewRotation);
		}

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(AIPawn);
		Params.AddIgnoredActor(TargetPlayer);

		// 디버그 라인을 그려서 시각적으로 확인합니다.
		DrawDebugLine(GetWorld(), TraceStart, AILocation, FColor::Orange, false, 0.2f, 0, 1.0f);

		const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, AILocation, ECC_Visibility, Params);

		if (bHit)
		{
			// 라인 트레이스가 무언가에 부딪혔다면, 그게 무엇인지 이름을 출력합니다.
			UE_LOG(LogTemp, Error, TEXT("Line Trace Hit: %s"), *HitResult.GetActor()->GetName());
		}

		if (!bHit || HitResult.GetActor() == AIPawn)
		{
			bIsCurrentlyVisible = true;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("최종 bIsCurrentlyVisible 값: %s"), bIsCurrentlyVisible ? TEXT("TRUE") : TEXT("FALSE"));
	BlackboardComp->SetValueAsBool(IsPlayerLookingKey.SelectedKeyName, bIsCurrentlyVisible);
}