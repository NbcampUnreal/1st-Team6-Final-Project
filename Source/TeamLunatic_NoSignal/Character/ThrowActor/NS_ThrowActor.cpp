#include "NS_ThrowActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"

ANS_ThrowActor::ANS_ThrowActor()
{
	PrimaryActorTick.bCanEverTick = false;

	BottleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BottleMesh"));
	RootComponent = BottleMesh;
	BottleMesh->SetCollisionProfileName("PhysicsActor");
	BottleMesh->SetIsReplicated(true);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = BottleMesh;
	ProjectileMovement->InitialSpeed = 1200.f;
	ProjectileMovement->MaxSpeed = 1200.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->ProjectileGravityScale = 1.0f;
	ProjectileMovement->bAutoActivate = false;

	bReplicates = true;
	SetReplicateMovement(false);
}

void ANS_ThrowActor::LaunchInDirection(const FVector& Direction)
{
	if (ProjectileMovement)
	{
		ProjectileMovement->Velocity = Direction * ProjectileMovement->InitialSpeed;
		ProjectileMovement->Activate();
	}
}
