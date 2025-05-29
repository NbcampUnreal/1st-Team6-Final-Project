#include "NS_StatusComponent.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
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
	AddStaminaRegenRate(1.5f);
    Stamina = MaxStamina;
    CurrentHunger  = MaxHunger;
    CurrentThirst  = MaxThirst;
    CurrentFatigue = MaxFatigue;
	PlayerCharacter = Cast<ANS_PlayerCharacterBase>(GetOwner());

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
	case EHungerStatus::Full: // 치료 아이템 추가 회복 배율(1.5배), 인벤토리무가 중량 추가(10), 근접 공격력&킥 공격력 증가(1.5배), 지속피해 없음
		PlayerCharacter->AddWeightInventory(10.f);
		GetWorld()->GetTimerManager().ClearTimer(DamagedByHungerTimerHandle);
		break;
	case EHungerStatus::Normal: // 치료 아이템 추가 회복 배율(1배), 인벤토리 무게 추가(0), 근접 공격력&킥 공격력 증가(1배), 지속피해 없음
		PlayerCharacter->AddWeightInventory(0.f);
		GetWorld()->GetTimerManager().ClearTimer(DamagedByHungerTimerHandle);
		break;
	case EHungerStatus::LittleHungry: // 치료 아이템 추가 회복 배율(0.8배), 인벤토리 무게 추가(0), 근접 공격력&킥 공격력 감소(0.8배), 지속피해 없음
		PlayerCharacter->AddWeightInventory(0.f);
		GetWorld()->GetTimerManager().ClearTimer(DamagedByHungerTimerHandle);
		break;
	case EHungerStatus::Hungry: // 치료 아이템 추가 회복 배율(0.5배), 인벤토리 무게 추가(-10), 근접 공격력&킥 공격력 감소(0.5배), 지속피해 없음
		PlayerCharacter->AddWeightInventory(-10.f);
		GetWorld()->GetTimerManager().ClearTimer(DamagedByHungerTimerHandle);
		break;
	case EHungerStatus::Starving: // 치료 아이템 추가 회복 배율(0.1배), 인벤토리 무게 추가(-10), 근접 공격력&킥 공격력 감소(0.1배), 지속피해 있음
		PlayerCharacter->AddWeightInventory(-10.f);
		GetWorld()->GetTimerManager().SetTimer(DamagedByHungerTimerHandle, this, &UNS_StatusComponent::DamagedByHunger, 5.f, true);
		break;
	}
}

void UNS_StatusComponent::ChangeStatusDueThirst(EThirstStatus NewStatus)
{
	ThirstStatus = NewStatus;
	// 상태에 따른 효과 적용
	switch (ThirstStatus)
	{
	case EThirstStatus::Full: //이동속도 증가(1.5배), 스태미너 재생 속도 증가(1.5배), 수색속도 증가(0.5배), 지속피해 없음
		PlayerCharacter->SetSpeedMultiply(1.5f);
		AddStaminaRegenRate(1.5f);
		PlayerCharacter->AddSearchTime(0.5f);
		GetWorld()->GetTimerManager().ClearTimer(DamagedByThirstTimerHandle);
		break;
	case EThirstStatus::Normal: //이동속도 증가(1배), 스태미너 재생 속도 증가(1배), 수색속도 증가(1배), 지속피해 없음
		PlayerCharacter->SetSpeedMultiply(1.0f);
		AddStaminaRegenRate(1.f);
		PlayerCharacter->AddSearchTime(1.f);
		GetWorld()->GetTimerManager().ClearTimer(DamagedByThirstTimerHandle);
		break;
	case EThirstStatus::LittleThirsty: //이동속도 증가(1.0배), 스태미너 재생 속도 증가(0.8배), 수색속도 증가(1.5배), 지속피해 없음
		PlayerCharacter->SetSpeedMultiply(1.0f);
		AddStaminaRegenRate(0.8f);
		PlayerCharacter->AddSearchTime(1.5f);
		GetWorld()->GetTimerManager().ClearTimer(DamagedByThirstTimerHandle);
		break;
	case EThirstStatus::Thirsty: //이동속도 증가(0.5배), 스태미너 재생 속도 증가(0.5배), 수색속도 증가(1.5배), 지속피해 없음
		PlayerCharacter->SetSpeedMultiply(0.5f);
		AddStaminaRegenRate(0.5f);
		PlayerCharacter->AddSearchTime(1.5f);
		GetWorld()->GetTimerManager().ClearTimer(DamagedByThirstTimerHandle);
		break;
	case EThirstStatus::Dehydrated: //이동속도 증가(0.5배), 스태미너 재생 속도 증가(0.1배), 수색속도 증가(1.5배), 지속피해 있음
		PlayerCharacter->SetSpeedMultiply(0.5f);
		AddStaminaRegenRate(0.1f);
		PlayerCharacter->AddSearchTime(1.5f);
		GetWorld()->GetTimerManager().SetTimer(DamagedByThirstTimerHandle, this, &UNS_StatusComponent::DamagedByThirst, 10.f, true);
		break;
	}
}

void UNS_StatusComponent::ChangeStatusDueFatigue(EFatigueStatus NewStatus)
{
	FatigueStatus = NewStatus;
	// 상태에 따른 효과 적용
	switch (FatigueStatus)
	{
	case EFatigueStatus::Rested: //(반동 또는 집탄률 좋게), 조준가능, 크래프팅 속도 증가(0.5배), (시야 또는 청각 방해 안함)
		PlayerCharacter->SetAvailableAiming(true);
		PlayerCharacter->AddCraftingSpeed(0.5f);
		break;
	case EFatigueStatus::Normal://(반동 또는 집탄률 보통), 조준가능, 크래프팅 속도 증가(1배), (시야 또는 청각 방해 안함)
		PlayerCharacter->SetAvailableAiming(true);
		PlayerCharacter->AddCraftingSpeed(1.f);
		break;
	case EFatigueStatus::Tired: //(반동 또는 집탄률 나쁘게), 조준가능, 크래프팅 속도 감소(1.5배), (시야 또는 청각 방해 안함)
		PlayerCharacter->SetAvailableAiming(true);
		PlayerCharacter->AddCraftingSpeed(1.5f);
		break;
	case EFatigueStatus::Exhausted: //(반동 또는 집탄률 나쁘게), 조준가능, 크래프팅 속도 감소(1.5배), (시야 또는 청각 방해 발동)
		PlayerCharacter->SetAvailableAiming(false);
		PlayerCharacter->AddCraftingSpeed(1.5f);
		break;
	case EFatigueStatus::Unconscious:
		//기절하기
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

void UNS_StatusComponent::AddStaminaRegenRate(float Value)
{
	StaminaRegenRate = DefalutStaminaRegenRate * Value; // 기본 재생 속도에 배율 적용
}
//================================================================

// 상태치에 따른 데미지 처리 함수들 ====
void UNS_StatusComponent::DamagedByHunger()
{
	UGameplayStatics::ApplyDamage(GetOwner(), 10.f, nullptr, nullptr, nullptr);
}

void UNS_StatusComponent::DamagedByThirst()
{
	UGameplayStatics::ApplyDamage(GetOwner(), 10.f, nullptr, nullptr, nullptr);
}
