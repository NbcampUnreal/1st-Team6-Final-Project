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
    UpdateMaxStamina();
    Stamina = MaxStamina;
    Hunger  = MaxHunger;
    Thirst  = MaxThirst;
    Fatigue = MaxFatigue;
}

// 스탯 컴포넌트의 TickComponent 함수에서 매 프레임마다 스탯을 업데이트
// 상태치를 업데이트 
// 상태치에 따른 버프/디버프를 적용
void UNS_StatusComponent::TickComponent(float DeltaTime,
                                       ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    UpdateAllStatus(DeltaTime);
    ApplyStageEffects(DeltaTime);
}

// 상태치 업데이트하는 함수
// 1) Hunger,Thirst,Fatigue 분당 감소
// 2) Stamina: Sprint 중 감소, 아닐 땐 재생
void UNS_StatusComponent::UpdateAllStatus(float DeltaTime)
{
    // 1) Hunger,Thirst,Fatigue 분당 감소
    const float HungerRate  = HungerDecreasePerMinute  / 60.f; // 60.0f 현재 디버그용으로 1로 설정
    const float ThirstRate  = ThirstDecreasePerMinute  / 60.f; // 60.0f 현재 디버그용으로 1로 설정
    const float FatigueRate = FatigueDecreasePerMinute / 60.f; // 60.0f 현재 디버그용으로 1로 설정

    DecreaseHunger(-HungerRate  * DeltaTime);
    DecreaseThirst(-ThirstRate  * DeltaTime);
    DecreaseFatigue(-FatigueRate * DeltaTime);

    // 2) Stamina: Sprint 중 감소, 아닐 땐 재생
    if (ANS_PlayerCharacterBase* PlayChar = Cast<ANS_PlayerCharacterBase>(GetOwner()))
    {
        
        float Change = SprintStaminaDecreasePerSecond * DeltaTime;

        //점프, 숙이기 상태에선 무시
        if (!PlayChar->GetMovementComponent()->IsMovingOnGround() || PlayChar->GetMovementComponent()->IsCrouching())
        {
            Change = 0.f;
        }

		// 스프린트 중일 때 스태미너 감소
		float SumVelocity = FMath::Abs(PlayChar->GetMovementComponent()->Velocity.X) + FMath::Abs(PlayChar->GetMovementComponent()->Velocity.Y);


        DecreaseStamina((PlayChar->IsSprint && !FMath::IsNearlyZero(SumVelocity)) ? -Change : +Change);


        if (PlayChar->IsSprint && Stamina <= 0.f)
        {
            PlayChar->IsSprint = false;
        }
    }
}

//     else if (Thirst > MaxThirst * 0.34f) {}
// 상태치에 따른 버프/디버프를 적용하는 함수
// 1) 허기, 목마름, 졸림 게이지에 따른 버프/디버프 적용
// 2) 체력 변화 적용
// 3) 이동속도 변화 적용
// 4) 기절 상태: Fatigue 강제 회복
void UNS_StatusComponent::ApplyStageEffects(float DeltaTime)
{
    float speedMult = 1.f;
    float hpChange  = 0.f;

    //// --- 허기 단계 ---
    //if      (Hunger > MaxHunger * 0.75f) { speedMult += 0.10f; hpChange += 2.f; }
    //else if (Hunger > MaxHunger * 0.34f) { hpChange += 1.f; }
    //else if (Hunger > MaxHunger * 0.10f) { speedMult -= 0.20f; }
    //else if (Hunger > 0.f)               { speedMult -= 0.50f; }
    //else                                  { speedMult -= 0.50f; hpChange -= 1.f; }

    //// --- 목마름 단계 ---
    //if      (Thirst > MaxThirst * 0.75f) { speedMult += 0.10f; }
    //else if (Thirst > MaxThirst * 0.34f) {}
    //else if (Thirst > MaxThirst * 0.10f) { speedMult -= 0.10f; }
    //else if (Thirst > 0.f)               { speedMult -= 0.30f; }
    //else                                 { speedMult -= 0.30f; hpChange -= 1.f; }

    //// --- 졸림 단계 ---
    //if      (Fatigue > MaxFatigue * 0.50f) {}
    //else if (Fatigue > MaxFatigue * 0.10f) { speedMult -= 0.10f; }
    //else if (Fatigue > 0.f)                { speedMult -= 0.20f; }
    //else
    //{
    //    // 기절 상태: Fatigue 강제 회복
    //    Fatigue += 5.f * DeltaTime;
    //    Fatigue = FMath::Clamp(Fatigue, 0.f, MaxFatigue);
    //}

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

    // 체력 변화 적용 ==> 나중에 TakeDamage로 대체
    ChangeHealthGauge(hpChange * DeltaTime);
}


// 현재 체력 변화에 따라 최대 스태미너를 업데이트하는 함수
void UNS_StatusComponent::UpdateMaxStamina()
{
	MaxStamina = FMath::Clamp((Health / MaxHealth) * 100, 0.f, 100.f);
}

//체력 증감 처리
void UNS_StatusComponent::ChangeHealthGauge(float Delta)
{
    Health = FMath::Clamp(Health + Delta, 0.f, MaxHealth);
    UpdateMaxStamina();
}

// 각 스탯 감소 제한===============================================

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
//================================================================