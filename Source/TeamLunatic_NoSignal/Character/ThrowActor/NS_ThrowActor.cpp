#include "Character/ThrowActor/NS_ThrowActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"

ANS_ThrowActor::ANS_ThrowActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	BottleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BottleMesh"));
	BottleMesh->SetCollisionProfileName("BlockAll"); // 필요시 Overlap로 바꿔도 됨
	BottleMesh->SetIsReplicated(true);
	RootComponent = BottleMesh;

	// 투척 궤도 속도 설정
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = BottleMesh;
	ProjectileMovement->InitialSpeed = 1200.f;
	ProjectileMovement->MaxSpeed = 1200.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->ProjectileGravityScale = 1.0f;
	ProjectileMovement->SetIsReplicated(true);

	// 네트워크 복제 설정
	bReplicates = true;
	SetReplicateMovement(false);
}

void ANS_ThrowActor::BeginPlay()
{
	Super::BeginPlay();
}
