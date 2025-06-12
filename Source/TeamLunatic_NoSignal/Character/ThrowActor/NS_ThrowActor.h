#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NS_ThrowActor.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_ThrowActor : public AActor
{
	GENERATED_BODY()

public:
	ANS_ThrowActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* BottleMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UProjectileMovementComponent* ProjectileMovement;

	// 병이 날아가는 방향 지정
	void LaunchInDirection(const FVector& Direction);
};
