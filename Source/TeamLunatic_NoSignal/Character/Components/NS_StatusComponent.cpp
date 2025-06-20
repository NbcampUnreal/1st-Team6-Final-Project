#include "NS_StatusComponent.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Net/UnrealNetwork.h>

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
	AddStaminaRegenRate(1.f); // 기본 재생 속도 설정
    Stamina = MaxStamina;
	PlayerCharacter = Cast<ANS_PlayerCharacterBase>(GetOwner());

}
void UNS_StatusComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UNS_StatusComponent, Health); // 체력
	DOREPLIFETIME(UNS_StatusComponent, MaxStamina); // 최대 스태미너
}

// 스탯 컴포넌트의 TickComponent 함수에서 매 프레임마다 스탯을 업데이트
// 상태치를 업데이트 
// 상태치에 따른 버프/디버프를 적용
void UNS_StatusComponent::TickComponent(float DeltaTime,
                                       ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateStamina(DeltaTime); // 스태미너 업데이트
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
			ChangingStaminaValue = StaminaDereaseRate * DeltaTime;
		}
		else // 서있는 상태 회복O
		{
			// 스프린트가 아닐 때는 재생 속도 적용
			ChangingStaminaValue = CurrentStaminaRegenRate * DeltaTime;
		}

		AddStamina(ChangingStaminaValue);

		if (bEnableSprint == false && Stamina * 10 >= MaxStamina)
		{
			bEnableSprint = true; // 스태미너가 충분하면 스프린트 활성화
		}

		// 스태미너가 0 이하로 떨어지면 스프린트 비활성화
		if (PlayChar->IsSprint && Stamina <= 0.f)
		{
			bEnableSprint = false;
		}
	}
}

// 현재 체력 변화에 따라 최대 스태미너를 업데이트하는 함수
void UNS_StatusComponent::UpdateMaxStamina()
{
	MaxStamina = FMath::Clamp((Health / MaxHealth) * 100, 0.f, 100.f);
}

void UNS_StatusComponent::AddHealthGauge_Implementation(float Value)
{
	Health = FMath::Clamp(Health + Value, 0.f, MaxHealth);
	UpdateMaxStamina();
}

// 각 스탯 변경===============================================

void UNS_StatusComponent::AddStamina(float Value)
{
    Stamina = FMath::Clamp(Stamina + Value, 0.f, MaxStamina);
	UE_LOG(LogTemp, Warning, TEXT("Current Stamina: %f"), Stamina); // 현재 스태미너 로그 출력
}

void UNS_StatusComponent::AddStaminaRegenRate(float Value)
{
	CurrentStaminaRegenRate = DefalutStaminaRegenRate * Value; // 기본 재생 속도에 배율 적용
}
bool UNS_StatusComponent::CheckEnableSprint()
{
	return bEnableSprint; // 스프린트 가능 여부 반환
}
//================================================================

