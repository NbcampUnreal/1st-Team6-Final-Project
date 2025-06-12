#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NS_ThrowActor.generated.h"

class UProjectileMovementComponent;
class UStaticMeshComponent;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_ThrowActor : public AActor
{
	GENERATED_BODY()

public:
	ANS_ThrowActor();

protected:
	virtual void BeginPlay() override;

public:
	// 병 메시
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* BottleMesh;

	// 투사체 이동 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	UProjectileMovementComponent* ProjectileMovement;
};
