#include "NS_ThrowActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Hearing.h"
#include "GeometryCollection/GeometryCollectionActor.h"

ANS_ThrowActor::ANS_ThrowActor()
{
	PrimaryActorTick.bCanEverTick = false;

	BottleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BottleMesh"));
	RootComponent = BottleMesh;
	BottleMesh->SetSimulatePhysics(true);
	BottleMesh->SetNotifyRigidBodyCollision(true);
	BottleMesh->SetCollisionProfileName("BlockAll");

	BottleMesh->OnComponentHit.AddDynamic(this, &ANS_ThrowActor::OnHit);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = BottleMesh;
	ProjectileMovement->InitialSpeed = 1200.f;
	ProjectileMovement->MaxSpeed = 1200.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
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

void ANS_ThrowActor::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
						   UPrimitiveComponent* OtherComp, FVector NormalImpulse,
						   const FHitResult& Hit)
{
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
	
	if (!FractureActorClass) return;

	// 지오메트리컬렉션 메쉬 스폰 === 실제 부서지는건 지오메트리 컬렉션에 설정값으로 부서짐
	if (FractureActorClass)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		if (auto* Fracture = GetWorld()->SpawnActor<AGeometryCollectionActor>(
				FractureActorClass,
				GetActorLocation(),
				GetActorRotation(),
				Params))
		{
			// 5초 뒤에 사라짐
			Fracture->SetLifeSpan(5.0f);
		}
	}

	// 병 액터는 땅에 닿는순간 바로 제거
	Destroy();
}