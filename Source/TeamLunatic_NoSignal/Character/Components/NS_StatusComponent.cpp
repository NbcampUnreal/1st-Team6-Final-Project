#include "NS_StatusComponent.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

// 이 컴포넌트의 기본 속성들을 설정합니다.
UNS_StatusComponent::UNS_StatusComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    // 네트워크 복제 활성화
    SetIsReplicatedByDefault(true);
}

void UNS_StatusComponent::BeginPlay()
{
    Super::BeginPlay();

    // 값 초기화
    Health = MaxHealth; // 현재 체력을 최대 체력으로
    Stamina = MaxStamina; // 현재 스태미너를 최대 스태미너로
	PlayerCharacter = Cast<ANS_PlayerCharacterBase>(GetOwner()); 
}

void UNS_StatusComponent::StartSprinting()
{
	bIsSprinting = true;
	GetWorld()->GetTimerManager().ClearTimer(StaminaTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(StaminaTimerHandle, this, &UNS_StatusComponent::UpdateStamina, 0.1f, true);
}

void UNS_StatusComponent::StopSprinting()
{
	bIsSprinting = false;
}

void UNS_StatusComponent::UpdateStamina()
{
	const int32 OldStamina = Stamina;

	if (bIsSprinting)
	{
		if (Stamina > 0)
		{
			Stamina = FMath::Max(0, Stamina + FMath::RoundToInt(StaminaDereaseRate * 0.1f));
		}
		else
		{
			StopSprinting();
			if (PlayerCharacter)
			{
				PlayerCharacter->StopSprint(FInputActionValue());
			}
		}
	}
	else
	{
		if (Stamina < MaxStamina)
		{
			Stamina = FMath::Min(MaxStamina, Stamina + FMath::RoundToInt(CurrentStaminaRegenRate * 0.1f));
		}
		else
		{
			GetWorld()->GetTimerManager().ClearTimer(StaminaTimerHandle);
		}
	}

	if (OldStamina != Stamina)
	{
		OnStaminaChanged.Broadcast(Stamina, MaxStamina);
	}

	if (bEnableSprint == false && Stamina > 10)
	{
		bEnableSprint = true;
	}

	if (bIsSprinting && Stamina <= 0)
	{
		bEnableSprint = false;
	}
}

void UNS_StatusComponent::UpdateHealthChange(float Value)
{
    const int32 OldHealth = Health;
    Health = FMath::Clamp(Health + FMath::RoundToInt(Value), 0, MaxHealth);

    // 체력이 실제로 변경되었다면
    if (OldHealth != Health)
    {
        //  체력 변경 델리게이트 방송 (UI 업데이트용)
        OnHealthChanged.Broadcast(Health, MaxHealth);

        // 피격 이벤트 델리게이트 방송 (사운드, 이펙트용)
        // Delta는 음수 값이므로 -를 붙여 양수 데미지 값으로 전달
        OnDamaged.Broadcast(-Value); 
    }

    // 사망 판정
    if (Health <= 0 && OldHealth > 0) // 이전 체력은 0보다 컸을 때만 
    {
        if (PlayerCharacter)
        {
            PlayerCharacter->PlayDeath_Server();
        }
    }
}

void UNS_StatusComponent::UpdateStaminaChange(float Value)
{
    int32 OldStamina = Stamina;
    Stamina = FMath::Clamp(Stamina + FMath::RoundToInt(Value), 0, MaxStamina);
    
    if (OldStamina != Stamina)
    {
        // 스태미너 변경 이벤트 발생
        OnStaminaChanged.Broadcast(Stamina, MaxStamina);
    }
}

bool UNS_StatusComponent::CheckEnableSprint()
{
	return bEnableSprint;
}

// 네트워크 복제 설정
void UNS_StatusComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // 체력과 스태미너 관련 변수들을 복제
    DOREPLIFETIME(UNS_StatusComponent, Health);
    DOREPLIFETIME(UNS_StatusComponent, MaxHealth);
    DOREPLIFETIME(UNS_StatusComponent, Stamina);
    DOREPLIFETIME(UNS_StatusComponent, MaxStamina);
}
//================================================================