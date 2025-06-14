#include "NS_ThrowActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h" // 추가: BoxComponent 포함
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Hearing.h"
#include "GeometryCollection/GeometryCollectionActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h" // 추가: GeometryCollectionComponent 사용 위함

ANS_ThrowActor::ANS_ThrowActor()
{
	PrimaryActorTick.bCanEverTick = false; // 충돌 방식 변경으로 Tick 필요 없어짐.

	// BottleMesh 설정
	BottleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BottleMesh"));
	RootComponent = BottleMesh;
	BottleMesh->SetSimulatePhysics(false); // Static Mesh 자체는 물리 시뮬레이션 하지 않음
	BottleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Static Mesh는 충돌 없음

	// 오버랩 감지를 위한 BoxCollisionComponent 추가 및 설정
	OverlapCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapCollision"));
	OverlapCollision->SetupAttachment(RootComponent);
	OverlapCollision->SetBoxExtent(FVector(50.f, 50.f, 50.f)); // 적절한 크기로 조절
	OverlapCollision->SetCollisionProfileName(TEXT("OverlapAll")); // 모든 것에 오버랩
	OverlapCollision->SetGenerateOverlapEvents(true); // 오버랩 이벤트 생성 활성화
	OverlapCollision->OnComponentBeginOverlap.AddDynamic(this, &ANS_ThrowActor::OnOverlapBegin);

	// ProjectileMovement 설정
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = BottleMesh; // BottleMesh의 위치를 업데이트
	ProjectileMovement->InitialSpeed = 1200.f;
	ProjectileMovement->MaxSpeed = 1200.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false; // 오버랩 방식으로 변경했으므로 더 이상 바운스 필요 없음
	ProjectileMovement->ProjectileGravityScale = 1.0f;

	bReplicates = true;
	SetReplicateMovement(true);
}

void ANS_ThrowActor::BeginPlay()
{
	Super::BeginPlay();

	// 필요한 경우 BeginPlay에서 OverlapCollision의 크기를 BottleMesh에 맞게 조절할 수 있습니다.
	// FVector BoundsExtent = BottleMesh->GetStaticMesh()->GetBounds().BoxExtent;
	// OverlapCollision->SetBoxExtent(BoundsExtent * 1.2f); // 메쉬보다 약간 크게 설정
}

void ANS_ThrowActor::LaunchInDirection(const FVector& Direction)
{
	if (ProjectileMovement)
	{
		ProjectileMovement->Velocity = Direction * ProjectileMovement->InitialSpeed;
		ProjectileMovement->Activate();
	}
}

// 기존 OnHit 함수 대신 사용될 OnOverlapBegin 함수
void ANS_ThrowActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 자기 자신과의 오버랩 방지
	if (OtherActor == this || OtherActor == GetOwner())
	{
		return;
	}

	// 파편화할 액터 클래스가 설정되어 있지 않다면 리턴
	if (!FractureActorClass) return;

	// 사운드 재생
	if (!bHasPlayedImpactSound)
	{
		if (ImpactSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
		}
		UAISense_Hearing::ReportNoiseEvent(
			GetWorld(),
			GetActorLocation(),
			1.0f,
			this,
			2000.f,
			NAME_None
		);
		bHasPlayedImpactSound = true;
	}

	// 현재 투사체의 마지막 위치와 속도를 사용하여 지오메트리 컬렉션 스폰
	FVector CurrentLocation = GetActorLocation();
	FVector CurrentVelocity = ProjectileMovement->Velocity;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // 항상 스폰

	AGeometryCollectionActor* SpawnedFractureActor = GetWorld()->SpawnActor<AGeometryCollectionActor>(
		FractureActorClass,
		CurrentLocation, // 현재 투사체의 위치에 스폰
		CurrentVelocity.Rotation(), // 현재 속도 방향으로 회전 (선택 사항)
		SpawnParams
	);

	if (SpawnedFractureActor)
	{
		UGeometryCollectionComponent* GeoComp = SpawnedFractureActor->GetGeometryCollectionComponent();
		if (GeoComp)
		{
			GeoComp->SetSimulatePhysics(true); // 스폰된 GeometryCollection은 물리 시뮬레이션 시작
			GeoComp->AddImpulse(CurrentVelocity * GeoComp->GetMass() * 0.1f); // 중요: 스폰된 Geometry Collection에 투사체의 속도 기반 충격량 부여

			// 또는 ApplyBreakingDamage를 사용하여 더 직접적인 파괴를 유도할 수 있습니다.
			// GeoComp->ApplyBreakingDamage(CurrentVelocity.Size() * 0.1f, CurrentLocation, CurrentVelocity.GetSafeNormal(), 0.f);
		}
	}

	// 원본 액터 (Static Mesh를 포함한 ANS_ThrowActor)를 파괴
	Destroy();
}