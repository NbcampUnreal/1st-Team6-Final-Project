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
	PrimaryActorTick.bCanEverTick = false; 

	// 병 스테틱메쉬
	BottleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BottleMesh"));
	RootComponent = BottleMesh;
	BottleMesh->SetSimulatePhysics(false); // 스테틱메쉬는 물리시뮬레이션 적용 안해주고
	BottleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 스테틱메쉬는 충돌필요없고 오버렙이벤트로만 충돌하게 설정하고있음

	// 박스콜리전 오버렙이벤트
	OverlapCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapCollision"));
	OverlapCollision->SetupAttachment(RootComponent);
	OverlapCollision->SetBoxExtent(FVector(50.f, 50.f, 50.f)); // 크기는 추가로 에디터에서 조정해줘야할듯
	OverlapCollision->SetCollisionProfileName(TEXT("OverlapAll")); // 모든 것에 오버랩하고
	OverlapCollision->SetGenerateOverlapEvents(true); // 오버랩 이벤트 생성 활성화해주고
	OverlapCollision->OnComponentBeginOverlap.AddDynamic(this, &ANS_ThrowActor::OnOverlapBegin);

	//날아가는 프로젝트일컴포넌트 설정
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = BottleMesh; // 스테틱메쉬에 위치를 업데이틓고
	ProjectileMovement->InitialSpeed = 1200.f;
	ProjectileMovement->MaxSpeed = 1200.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false; // 오버렙방식이라 바운드는 적용안하고
	ProjectileMovement->ProjectileGravityScale = 1.0f;

	bReplicates = true;
	SetReplicateMovement(true);
}

void ANS_ThrowActor::BeginPlay()
{
	Super::BeginPlay();
}

void ANS_ThrowActor::LaunchInDirection(const FVector& Direction)
{
	if (ProjectileMovement)
	{
		ProjectileMovement->Velocity = Direction * ProjectileMovement->InitialSpeed;
		ProjectileMovement->Activate();
	}
}

void ANS_ThrowActor::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	// 자신 또는 소유자는 무시하고
	if (OtherActor == this || OtherActor == GetOwner())
		return;

	if (!FractureActorClass)
		return;

	// 병깨지는 사운드는 1번만 재생되도록 하고
	if (!bHasPlayedImpactSound)
	{
		if (ImpactSound)
			UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 1.f, this, 2000.f, NAME_None);
		bHasPlayedImpactSound = true;
	}

	// 현재 오버렙된 위치랑 기존 속도를 가져오고
	FVector SpawnLocation = GetActorLocation();
	FVector CurrentVelocity = ProjectileMovement->Velocity;

	// Z축 밑으로 떨어지는 속도는 0으로 설정해서 액터를 뚫고 아래로 안내려가도록해야함
	CurrentVelocity.Z = FMath::Max(0.f, CurrentVelocity.Z);

	// 기존 스테틱 메쉬가 날아가는 속도에 곱해줄 값
	const float SpeedScale = 0.025f;
	FVector ReducedVelocity = CurrentVelocity * SpeedScale;

	// 지오메트레컬렉션 액터 스폰
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// 스폰액터에 속도는 기존에 던져지던 스테틱메쉬에 속도에 0.025배로 줄여서 실제로 병이 깨지는것처럼 구현
	AGeometryCollectionActor* FractureActor = GetWorld()->SpawnActor<AGeometryCollectionActor>(
		FractureActorClass,
		SpawnLocation,
		ReducedVelocity.Rotation(),
		SpawnParams
	);

	if (FractureActor)
	{
		UGeometryCollectionComponent* GeoComp = FractureActor->GetGeometryCollectionComponent();
		if (GeoComp)
		{
			GeoComp->SetSimulatePhysics(true);
			float Mass = GeoComp->GetMass();
			GeoComp->AddImpulse(ReducedVelocity * Mass);
		}

		// 스폰타임은 5초로 설정
		FractureActor->SetLifeSpan(5.f);
	}

	// 원본 투사체 제거
	Destroy();
}