#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GeometryCollection/GeometryCollectionActor.h"
#include "NS_ThrowActor.generated.h"

class UStaticMeshComponent;
class UProjectileMovementComponent;
class UGeometryCollectionComponent;
class UBoxComponent; // 추가: BoxCollisionComponent를 사용하기 위함

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_ThrowActor : public AActor
{
	GENERATED_BODY()

public:
	ANS_ThrowActor();

	// 날아갈 병 메쉬
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* BottleMesh;

	// 추가: 오버랩 감지를 위한 박스 콜리전
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UBoxComponent* OverlapCollision;

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

	void LaunchInDirection(const FVector& Direction);
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};