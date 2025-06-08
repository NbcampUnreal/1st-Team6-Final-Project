#include "NS_Trap.h"
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

void ANS_Trap::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
}

void ANS_Trap::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || (bHasPlayed && bPlayOnce))
        return;

    if (HasAuthority())
    {
        Multicast_PlayTrapSound();

        APawn* InstigatorPawn = Cast<APawn>(OtherActor);
        if (InstigatorPawn)
        {
            UAISense_Hearing::ReportNoiseEvent(
                GetWorld(),
                GetActorLocation(),
                1.0f,
                InstigatorPawn,
                0.0f,
                FName("TrapNoise")
            );
        }

        bHasPlayed = true;
    }
}

void ANS_Trap::Multicast_PlayTrapSound_Implementation()
{
    if (TrapSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, TrapSound, GetActorLocation());
    }
}
