#include "Character/Components/NS_StatusComponent.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

// 이 컴포넌트의 기본 속성들을 설정합니다.
UNS_StatusComponent::UNS_StatusComponent()
{
	// 이 컴포넌트는 Tick 함수를 직접 사용하지 않고, 필요한 주기적인 업데이트는 Timer를 사용합니다.
	// Tick을 비활성화하면 매 프레임마다 불필요한 함수 호출을 막아 성능에 도움이 됩니다.
	PrimaryComponentTick.bCanEverTick = false;

	// 컴포넌트가 기본적으로 네트워크 복제를 지원하도록 설정합니다.
	// 멀티플레이어 환경에서 서버의 상태 값이 클라이언트에 자동으로 동기화되도록 합니다.
	SetIsReplicatedByDefault(true);
}

// 컴포넌트가 월드에 배치되고 게임이 시작될 때 호출됩니다.
void UNS_StatusComponent::BeginPlay()
{
	Super::BeginPlay();


	// 값 초기화
	Health = MaxHealth;   // 현재 체력을 최대 체력으로 설정
	Stamina = MaxStamina; // 현재 스태미너를 최대 스태미너로 설정
	CurrentStaminaRegenRate = DefalutStaminaRegenRate; // 스태미너 재생 속도를 기본값으로 초기화
	SprintMultiply = 1.5f;


	// 컴포넌트의 소유자(Owner)를 PlayerCharacter 타입으로 캐스팅하여 저장해둡니다.
	// 매번 캐스팅하는 비용을 줄이기 위함입니다.
	PlayerCharacter = Cast<ANS_PlayerCharacterBase>(GetOwner());

	// 서버에서만 스태미나 업데이트 타이머를 설정합니다.
	// 0.1초마다 UpdateStamina 함수를 반복적으로 호출합니다.
	if (GetOwner()->HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(
			StaminaTimerHandle,
			this,
			&UNS_StatusComponent::UpdateStamina,
			0.1f,
			true);
	}
}

// 달리기 상태를 시작합니다.
void UNS_StatusComponent::StartSprinting()
{
	// 스태미나가 10 이상이고 달리기가 가능할 때만 시작
	if (Stamina >= 10 && EnableSprint)
	{
		bIsSprinting = true;
		PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = PlayerCharacter->DefaultWalkSpeed * SprintMultiply;
	}
}

// 달리기 상태를 중지합니다.
void UNS_StatusComponent::StopSprinting()
{
	bIsSprinting = false;

	PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = PlayerCharacter->DefaultWalkSpeed;
}

void UNS_StatusComponent::UpdateStamina()
{
	if (bIsSprinting)
	{
		// 달리는 중 스태미나 감소
		Stamina = FMath::Max(0, Stamina - FMath::RoundToInt(FMath::Abs(StaminaDereaseRate) * 0.1f));
		
		// 스태미나가 0이 되면 달리기 중지
		if (Stamina <= 0)
		{
			StopSprinting();
			EnableSprint = false;
		}
	}
	else
	{
		// 달리지 않을 때 스태미나 회복
		if (Stamina < MaxStamina)
		{
			Stamina = FMath::Min(MaxStamina, Stamina + FMath::RoundToInt(CurrentStaminaRegenRate * 0.1f));
		}
		
		// 스태미나가 10 이상이 되면 다시 달릴 수 있음
		if (Stamina >= 10)
		{
			EnableSprint = true;
		}
	}
}

// 외부 요인(데미지, 힐)에 의해 체력을 변경합니다. 서버에서 호출되어야 합니다.
void UNS_StatusComponent::UpdateHealthChange(float Value)
{
	const int32 OldHealth = Health;
	// Value는 양수(힐) 또는 음수(데미지)일 수 있습니다. 체력은 0과 MaxHealth 사이로 제한됩니다.
	Health = FMath::Clamp(Health + FMath::RoundToInt(Value), 0, MaxHealth);

	// 체력이 실제로 변경되었다면
	if (OldHealth != Health)
	{
		// UI 업데이트 등을 위해 체력 변경 델리게이트를 방송합니다.
		OnHealthChanged.Broadcast(Health, MaxHealth);

		// 데미지를 입은 경우 (Value가 음수일 때)
		if (Value < 0.0f)
		{
			// 피격 이벤트 델리게이트를 방송합니다. (사운드, 이펙트용)
			// Delta는 음수 값이므로 -를 붙여 양수인 순수 데미지 값으로 전달합니다.
			OnDamaged.Broadcast(-Value);
		}
	}

	// 사망 판정: 이전 체력은 0보다 컸는데, 현재 체력이 0 이하일 때만 사망 로직을 실행합니다. (중복 실행 방지)
	if (Health <= 0 && OldHealth > 0)
	{
		if (PlayerCharacter)
		{
			// 캐릭터에게 사망 처리를 요청합니다. 서버에서 실행되어야 합니다.
			PlayerCharacter->PlayDeath_Server();
		}
	}
}

// 외부 요인(아이템 사용 등)에 의해 스태미나를 즉시 변경합니다.
void UNS_StatusComponent::UpdateStaminaChange(float Value)
{
	int32 OldStamina = Stamina;
	Stamina = FMath::Clamp(Stamina + FMath::RoundToInt(Value), 0, MaxStamina);

	if (OldStamina != Stamina)
	{
		// 스태미너 변경 이벤트 델리게이트를 방송합니다.
		OnStaminaChanged.Broadcast(Stamina, MaxStamina);
	}
}

// 네트워크를 통해 복제(Replicate)할 변수들을 정의합니다.
void UNS_StatusComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 아래 변수들은 서버에서 값이 변경되면 모든 클라이언트로 자동으로 전송됩니다.
	DOREPLIFETIME(UNS_StatusComponent, Health);       // 현재 체력
	DOREPLIFETIME(UNS_StatusComponent, MaxHealth);    // 최대 체력
	DOREPLIFETIME(UNS_StatusComponent, Stamina);      // 현재 스태미나
	DOREPLIFETIME(UNS_StatusComponent, MaxStamina);   // 최대 스태미나
	DOREPLIFETIME(UNS_StatusComponent, bIsSprinting); // 달리기 상태 (애니메이션 동기화 등에 사용될 수 있음)
}
