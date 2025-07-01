#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NS_ZombieSpawner.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API AANS_ZombieSpawner : public AActor
{
	GENERATED_BODY()

public:    
	// Sets default values for this actor's properties
	AANS_ZombieSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:    
	// 스폰 지점 내에서 좀비를 스폰할 위치를 반환하는 함수 (오버라이딩 가능)
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	FVector GetRandomSpawnLocationInBounds();

	// 이 스포너가 활성화되었는지 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	bool bIsEnabled = true;

	// 시각적으로 스폰 범위를 표현하기 위한 컴포넌트 (디버그용)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn")
	class UBoxComponent* SpawnBounds; // 또는 USphereComponent

};