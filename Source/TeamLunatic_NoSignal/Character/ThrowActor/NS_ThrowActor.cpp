#include "NS_ThrowActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"

ANS_ThrowActor::ANS_ThrowActor()
{
	PrimaryActorTick.bCanEverTick = false;

	BottleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BottleMesh"));
	RootComponent = BottleMesh;
	
	BottleMesh->SetCollisionProfileName("PhysicsActor");
	BottleMesh->SetSimulatePhysics(true);
	BottleMesh->SetEnableGravity(true);
	BottleMesh->SetIsReplicated(true);

	bReplicates = true;
	SetReplicateMovement(false);
}

