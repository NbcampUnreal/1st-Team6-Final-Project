#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GeometryCollection/GeometryCollectionActor.h"
#include "NS_ThrowActor.generated.h"

class UStaticMeshComponent;
class UProjectileMovementComponent;
class UGeometryCollectionComponent;
class UBoxComponent;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_ThrowActor : public AActor
{
	GENERATED_BODY()

public:
	ANS_ThrowActor();

	// 날아갈 병 메쉬
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* BottleMesh;

	// 날아갈 궤적 컴포넌트 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UProjectileMovementComponent* ProjectileMovement;

	// 땅이나 어디든 부딪쳤을때 부서질 지오메트리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throw")
	TSubclassOf<AGeometryCollectionActor> FractureActorClass;

	// Hit오버렙 이벤트에서 출력될 사운드 변수
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* ImpactSound;

	// 사운드 재생 여부 변수
	bool bHasPlayedImpactSound = false;

protected:
	
	virtual void BeginPlay() override;
public:
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
			   UPrimitiveComponent* OtherComp, FVector NormalImpulse,
			   const FHitResult& Hit);

	// 병이 날아가는 방향 지정
	void LaunchInDirection(const FVector& Direction);
};
