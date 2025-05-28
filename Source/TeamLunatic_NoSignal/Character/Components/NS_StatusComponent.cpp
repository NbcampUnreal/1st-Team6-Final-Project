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
    CurrentHunger  = MaxHunger;
    CurrentThirst  = MaxThirst;
    CurrentFatigue = MaxFatigue;
}

// 스탯 컴포넌트의 TickComponent 함수에서 매 프레임마다 스탯을 업데이트
// 상태치를 업데이트 
// 상태치에 따른 버프/디버프를 적용
void UNS_StatusComponent::TickComponent(float DeltaTime,
                                       ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    UpdateAllStatus(DeltaTime); // 상태들 가감하기
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

    AddHunger(-HungerRate  * DeltaTime);
    AddThirst(-ThirstRate  * DeltaTime);
    AddFatigue(-FatigueRate * DeltaTime);

    UpdateStatusEnums(); // 상태치에 따른 Enum변수 업데이트
	UpdateStamina(DeltaTime); // 스태미너 업데이트

    
}

void UNS_StatusComponent::UpdateStatusEnums()
{    // --- 허기 단계 ---
	EHungerStatus ChangingHungerStatus = HungerStatus;
	if (CurrentHunger > MaxHunger * 0.75f) ChangingHungerStatus = EHungerStatus::Full;
	else if (CurrentHunger > MaxHunger * 0.50f) ChangingHungerStatus = EHungerStatus::Normal;
	else if (CurrentHunger > MaxHunger * 0.25f) ChangingHungerStatus = EHungerStatus::LittleHungry;
	else if (CurrentHunger > 0.f) ChangingHungerStatus = EHungerStatus::Hungry;
	else ChangingHungerStatus = EHungerStatus::Starving;
    //허기 상태 변경시 
	if (ChangingHungerStatus != HungerStatus)
        ChangeStatusDueHunger(ChangingHungerStatus);


	// --- 목마름 단계 ---
	EThirstStatus ChangingThirstStatus = ThirstStatus;
	if (CurrentThirst > MaxThirst * 0.75f) ChangingThirstStatus = EThirstStatus::Full;
	else if (CurrentThirst > MaxThirst * 0.50f) ChangingThirstStatus = EThirstStatus::Normal;
	else if (CurrentThirst > MaxThirst * 0.25f) ChangingThirstStatus = EThirstStatus::LittleThirsty;
	else if (CurrentThirst > 0.f) ChangingThirstStatus = EThirstStatus::Thirsty;
	else ChangingThirstStatus = EThirstStatus::Dehydrated;
	// 목마름 상태 변경시
	if (ChangingThirstStatus != ThirstStatus)
		ChangeStatusDueThirst(ChangingThirstStatus);

	// --- 졸림 단계 ---
	EFatigueStatus ChangingFatigueStatus = FatigueStatus;
    if (CurrentFatigue > MaxFatigue * 0.75f) ChangingFatigueStatus = EFatigueStatus::Rested;
    else if (CurrentFatigue > MaxFatigue * 0.50f) ChangingFatigueStatus = EFatigueStatus::Normal;
    else if (CurrentFatigue > MaxFatigue * 0.25f) ChangingFatigueStatus = EFatigueStatus::Tired;
    else if (CurrentFatigue > 0.f) ChangingFatigueStatus = EFatigueStatus::Exhausted;
    else ChangingFatigueStatus = EFatigueStatus::Unconscious;
	// 졸림 상태 변경시
	if (ChangingFatigueStatus != FatigueStatus)
		ChangeStatusDueFatigue(ChangingFatigueStatus);
}

void UNS_StatusComponent::ChangeStatusDueHunger(EHungerStatus NewStatus)
{
	HungerStatus = NewStatus;
	// 상태에 따른 효과 적용
	switch (HungerStatus)
	{
	case EHungerStatus::Full:
		break;
	case EHungerStatus::Normal:
		break;
	case EHungerStatus::LittleHungry:
		break;
	case EHungerStatus::Hungry:
		break;
	case EHungerStatus::Starving:
		break;
	}
}

void UNS_StatusComponent::ChangeStatusDueThirst(EThirstStatus NewStatus)
{
	ThirstStatus = NewStatus;
	// 상태에 따른 효과 적용
	switch (ThirstStatus)
	{
	case EThirstStatus::Full:
		break;
	case EThirstStatus::Normal:
		break;
	case EThirstStatus::LittleThirsty:
		break;
	case EThirstStatus::Thirsty:
		break;
	case EThirstStatus::Dehydrated:
		break;
	}
}

void UNS_StatusComponent::ChangeStatusDueFatigue(EFatigueStatus NewStatus)
{
	FatigueStatus = NewStatus;
	// 상태에 따른 효과 적용
	switch (FatigueStatus)
	{
	case EFatigueStatus::Rested:
		break;
	case EFatigueStatus::Normal:
		break;
	case EFatigueStatus::Tired:
		break;
	case EFatigueStatus::Exhausted:
		break;
	case EFatigueStatus::Unconscious:
		break;
	}
}

// 스태미너 업데이트 함수
// 스프린트 중일 때는 감소, 아닐 땐 재생
// 점프 및 숙이기 상태에서는 스태미너 변화 없음
void UNS_StatusComponent::UpdateStamina(float DeltaTime)
{
	if (ANS_PlayerCharacterBase* PlayChar = Cast<ANS_PlayerCharacterBase>(GetOwner()))
	{

		float ChangingStaminaValue;
		float SumVelocity = FMath::Abs(PlayChar->GetMovementComponent()->Velocity.X) + FMath::Abs(PlayChar->GetMovementComponent()->Velocity.Y);


		if (!PlayChar->GetMovementComponent()->IsMovingOnGround() || PlayChar->GetMovementComponent()->IsCrouching()) //점프, 숙이기 상태에선 회복X, 소모X
		{
			ChangingStaminaValue = 0.f;
		}
		else if (PlayChar->IsSprint && !FMath::IsNearlyZero(SumVelocity)) // 달리는 상황 소모O
		{
			ChangingStaminaValue = SprintStaminaDecreasePerSecond * DeltaTime;
		}
		else // 서있는 상태 회복O
		{
			// 스프린트가 아닐 때는 재생 속도 적용
			ChangingStaminaValue = StaminaRegenRate * DeltaTime;
		}
		AddStamina(ChangingStaminaValue);

		// 스태미너가 0 이하로 떨어지면 스프린트 비활성화
		if (PlayChar->IsSprint && Stamina <= 0.f)
		{
			PlayChar->IsSprint = false;
		}
	}
}

// 현재 체력 변화에 따라 최대 스태미너를 업데이트하는 함수
void UNS_StatusComponent::UpdateMaxStamina()
{
	MaxStamina = FMath::Clamp((Health / MaxHealth) * 100, 0.f, 100.f);
}

//체력 증감 처리
void UNS_StatusComponent::AddHealthGauge(float Value)
{
    Health = FMath::Clamp(Health + Value, 0.f, MaxHealth);
    UpdateMaxStamina();
}

// 각 스탯 변경===============================================

void UNS_StatusComponent::AddStamina(float Value)
{
    Stamina = FMath::Clamp(Stamina + Value, 0.f, MaxStamina);
}

void UNS_StatusComponent::AddHunger(float Value)
{
    CurrentHunger = FMath::Clamp(CurrentHunger + Value, 0.f, MaxHunger);
}

void UNS_StatusComponent::AddThirst(float Value)
{
    CurrentThirst = FMath::Clamp(CurrentThirst + Value, 0.f, MaxThirst);
}

void UNS_StatusComponent::AddFatigue(float Value)
{
    CurrentFatigue = FMath::Clamp(CurrentFatigue + Value, 0.f, MaxFatigue);
}
//================================================================