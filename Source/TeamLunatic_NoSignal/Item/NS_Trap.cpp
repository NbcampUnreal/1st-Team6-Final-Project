#include "NS_Trap.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ANS_Trap::ANS_Trap()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    RootComponent = TriggerBox;
    TriggerBox->SetCollisionProfileName(TEXT("Trigger"));

    TrapMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrapMesh"));
    TrapMesh->SetupAttachment(RootComponent);

    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ANS_Trap::OnOverlapBegin);
}

void ANS_Trap::BeginPlay()
{
    Super::BeginPlay();
}

void ANS_Trap::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (!HasAuthority()) return;
    if (!TrapSound || !OtherActor || OtherActor == this) return;
    if (bPlayOnce && bHasPlayed) return;

    bHasPlayed = true;
    Multicast_PlayTrapSound();
}

void ANS_Trap::Multicast_PlayTrapSound_Implementation()
{
    if (TrapSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, TrapSound, GetActorLocation());
    }
}
