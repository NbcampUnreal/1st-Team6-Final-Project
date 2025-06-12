#include "NS_ThrowActor.h"

#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GeometryCollection/GeometryCollectionActor.h"
#include "Kismet/GameplayStatics.h"

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
	if (!FractureActorClass) return;

	// GC 액터 스폰
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AGeometryCollectionActor* FractureActor = GetWorld()->SpawnActor<AGeometryCollectionActor>(
		FractureActorClass,
		GetActorLocation(),
		GetActorRotation(),
		Params
	);
	

	// 병 액터 제거
	Destroy();
}