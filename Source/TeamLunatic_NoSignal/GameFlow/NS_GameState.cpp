#include "NS_GameState.h"
#include "Net/UnrealNetwork.h"
#include "UObject/UnrealType.h"
#include "EngineUtils.h" // For TActorIterator

ANS_GameState::ANS_GameState()
{
    bIsNight = true;
    TimeOfDay = 0.0f;
    PrimaryActorTick.bCanEverTick = true;
}

void ANS_GameState::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    UE_LOG(LogTemp, Warning, TEXT("✅ GameState Tick Running"));
    UpdateTimeFromDaySequence();
}

void ANS_GameState::UpdateTimeFromDaySequence()
{
    for (TActorIterator<AActor> ActorIt(GetWorld()); ActorIt; ++ActorIt)
    {
        AActor* Actor = *ActorIt;
        if (!Actor) continue;

        // SunMoonDaySequenceActor 클래스 이름 확인
        const FString ClassName = Actor->GetClass()->GetName();
        if (!ClassName.Contains(TEXT("SunMoonDaySequenceActor")))
        {
            continue;
        }

        UE_LOG(LogTemp, Warning, TEXT("🔍 Found SunMoon Actor: %s (Class: %s)"), *Actor->GetName(), *ClassName);

        // float 변수 중 이름에 Time이 포함된 것 탐색
        float CurrentTime = -1.0f;
        FString MatchedPropName;

        for (TFieldIterator<FFloatProperty> PropIt(Actor->GetClass()); PropIt; ++PropIt)
        {
            const FString PropName = PropIt->GetName();
            if (PropName.Contains(TEXT("Time")))
            {
                CurrentTime = PropIt->GetPropertyValue_InContainer(Actor);
                MatchedPropName = PropName;
                break;
            }
        }

        if (CurrentTime < 0.0f)
        {
            UE_LOG(LogTemp, Error, TEXT("❌ No usable float Time property found in %s"), *ClassName);
            return;
        }

        TimeOfDay = CurrentTime;
        UE_LOG(LogTemp, Warning, TEXT("✅ [%s] Current Time: %.2f"), *MatchedPropName, TimeOfDay);

        // 밤/낮 판정
        const bool bNewIsNight = !(TimeOfDay >= 7.5f && TimeOfDay < 18.5f);
        if (bNewIsNight != bIsNight)
        {
            bIsNight = bNewIsNight;

            if (HasAuthority())
            {
                OnRep_IsNight();
            }
        }

        break; // 첫 번째 SunMoonDaySequenceActor만 사용
    }
}


void ANS_GameState::OnRep_IsNight()
{
    UE_LOG(LogTemp, Log, TEXT("[GameState] It is now %s."), bIsNight ? TEXT("Night") : TEXT("Day"));
}

void ANS_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ANS_GameState, bIsNight);
}
