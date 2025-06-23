#include "Zombie/ZombieSpawner/NS_ZombieSpawner.h"
#include "Components/BoxComponent.h" // UBoxComponent 사용 시
// #include "Components/SphereComponent.h" // USphereComponent 사용 시 (선택)
#include "Kismet/KismetMathLibrary.h" // FMath::RandPointInBox / RandPointInSphere 사용 시

// Sets default values
AANS_ZombieSpawner::AANS_ZombieSpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// 스폰 범위를 시각적으로 나타내는 컴포넌트 추가
	SpawnBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnBounds"));
	RootComponent = SpawnBounds; // RootComponent로 설정
	SpawnBounds->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 충돌은 필요 없음
	SpawnBounds->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f)); // 기본 크기 설정 (에디터에서 조절)
	// SpawnBounds->SetHiddenInGame(false); // 인게임에서 보이게 (디버그용)
	// SpawnBounds->SetVisibility(true); // 에디터에서 보이게
}

// Called when the game starts or when spawned
void AANS_ZombieSpawner::BeginPlay()
{
	Super::BeginPlay();
    
}

FVector AANS_ZombieSpawner::GetRandomSpawnLocationInBounds()
{
	// 스포너 액터의 현재 위치 (Bounds의 중심)
	FVector Origin = GetActorLocation();
	// BoxComponent의 절반 크기 (에디터에서 스케일 조절 시 자동으로 반영됨)
	FVector Extent = SpawnBounds->GetScaledBoxExtent(); // 스케일이 적용된 Extent

	// Box Component 범위 내에서 랜덤 포인트 생성
	return UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);

	// 만약 USphereComponent를 사용했다면:
	// float Radius = SpawnBounds->GetScaledSphereRadius();
	// return Origin + UKismetMathLibrary::RandomUnitVector() * FMath::RandRange(0.0f, Radius);
}