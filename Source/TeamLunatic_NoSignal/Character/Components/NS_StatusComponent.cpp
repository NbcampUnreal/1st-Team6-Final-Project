#include "NS_StatusComponent.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UNS_StatusComponent::UNS_StatusComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UNS_StatusComponent::BeginPlay()
{
    Super::BeginPlay();

    // 기존값 초기화
    Health  = MaxHealth;
    Stamina = MaxStamina;
    Hunger  = MaxHunger;
    Thirst  = MaxThirst;
    Fatigue = MaxFatigue;
}

void UNS_StatusComponent::TickComponent(float DeltaTime,
                                       ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    UpdateAllStatus(DeltaTime);
    ApplyStageEffects(DeltaTime);
}

void UNS_StatusComponent::UpdateAllStatus(float DeltaTime)
{
    // 1) Hunger,Thirst,Fatigue 분당 감소
    const float HungerRate  = HungerDecreasePerMinute  / 60.0f;
    const float ThirstRate  = ThirstDecreasePerMinute  / 60.0f;
    const float FatigueRate = FatigueDecreasePerMinute / 60.0f;

    DecreaseHunger(-HungerRate  * DeltaTime);
    DecreaseThirst(-ThirstRate  * DeltaTime);
    DecreaseFatigue(-FatigueRate * DeltaTime);

    // 2) Stamina: Sprint 중 감소, 아닐 땐 재생
    if (ANS_PlayerCharacterBase* PlayChar = Cast<ANS_PlayerCharacterBase>(GetOwner()))
    {
        float Change = SprintStaminaDecreasePerSecond * DeltaTime;
        DecreaseStamina(PlayChar->IsSprint ? -Change : +Change);

        if (PlayChar->IsSprint && Stamina <= 0.f)
        {
            PlayChar->IsSprint = false;
        }
    }
}

void UNS_StatusComponent::ApplyStageEffects(float DeltaTime)
{
    float speedMult = 1.f;
    float hpChange  = 0.f;

    // --- 허기 단계 ---
    if      (Hunger > MaxHunger * 0.75f) { speedMult += 0.10f; hpChange += 2.f; }
    else if (Hunger > MaxHunger * 0.34f) { hpChange += 1.f; }
    else if (Hunger > MaxHunger * 0.10f) { speedMult -= 0.20f; }
    else if (Hunger > 0.f)               { speedMult -= 0.50f; }
    else                                  { speedMult -= 0.50f; hpChange -= 1.f; }

    // --- 목마름 단계 ---
    if      (Thirst > MaxThirst * 0.75f) { speedMult += 0.10f; }
    else if (Thirst > MaxThirst * 0.34f) {}
    else if (Thirst > MaxThirst * 0.10f) { speedMult -= 0.10f; }
    else if (Thirst > 0.f)               { speedMult -= 0.30f; }
    else                                 { speedMult -= 0.30f; hpChange -= 1.f; }

    // --- 졸림 단계 ---
    if      (Fatigue > MaxFatigue * 0.50f) {}
    else if (Fatigue > MaxFatigue * 0.10f) { speedMult -= 0.10f; }
    else if (Fatigue > 0.f)                { speedMult -= 0.20f; }
    else
    {
        // 기절 상태: Fatigue 강제 회복
        Fatigue += 5.f * DeltaTime;
        Fatigue = FMath::Clamp(Fatigue, 0.f, MaxFatigue);
    }

    // 최종 이동속도 적용 
    if (ANS_PlayerCharacterBase* PC = Cast<ANS_PlayerCharacterBase>(GetOwner()))
    {
        if (auto* MoveComp = PC->GetCharacterMovement())
        {
            float baseSpeed = PC->IsSprint
                                ? PC->DefaultWalkSpeed * PC->SprintSpeedMultiplier
                                : PC->DefaultWalkSpeed;

            MoveComp->MaxWalkSpeed = baseSpeed * FMath::Max(speedMult, 0.f);
        }
    }

    // 체력 변화 적용 
    DecreaseHealth(hpChange * DeltaTime);
}

// 각 스탯 감소 제한
void UNS_StatusComponent::DecreaseHealth(float Delta)
{
    Health = FMath::Clamp(Health + Delta, 0.f, MaxHealth);
}

void UNS_StatusComponent::DecreaseStamina(float Delta)
{
    Stamina = FMath::Clamp(Stamina + Delta, 0.f, MaxStamina);
}

void UNS_StatusComponent::DecreaseHunger(float Delta)
{
    Hunger = FMath::Clamp(Hunger + Delta, 0.f, MaxHunger);
}

void UNS_StatusComponent::DecreaseThirst(float Delta)
{
    Thirst = FMath::Clamp(Thirst + Delta, 0.f, MaxThirst);
}

void UNS_StatusComponent::DecreaseFatigue(float Delta)
{
    Fatigue = FMath::Clamp(Fatigue + Delta, 0.f, MaxFatigue);
}
