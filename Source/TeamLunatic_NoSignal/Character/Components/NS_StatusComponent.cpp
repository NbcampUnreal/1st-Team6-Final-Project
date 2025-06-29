#include "NS_StatusComponent.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "TimerManager.h" 
#include "Kismet/GameplayStatics.h" // 이 파일에서 사용되지 않으므로, 게임플레이 스태틱이 사용되지 않으면 제거를 고려하십시오.
#include "GameFramework/CharacterMovementComponent.h"

// 이 컴포넌트의 기본 속성들을 설정합니다.
UNS_StatusComponent::UNS_StatusComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
}

// 게임이 시작될 때 호출됩니다.
void UNS_StatusComponent::BeginPlay()
{
    Super::BeginPlay();

    // 값 초기화
    Health  = MaxHealth; // 현재 체력을 최대 체력으로
    Stamina = MaxStamina; // 현재 스태미너를 최대 스태미너로
	AddStaminaRegenRate(1.f); // 기본 스태미너 재생 속도를 설정 (배율 1.0)
	PlayerCharacter = Cast<ANS_PlayerCharacterBase>(GetOwner()); 
}


void UNS_StatusComponent::TickComponent(float DeltaTime,
                                       ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateStamina(DeltaTime); // 매 프레임마다 스태미너를 업데이트
}

// 스태미너 업데이트 함수
// 스프린트 중일 때는 감소하고, 아닐 때는 재생
// 달리고 있거나 점프중일때는 스태미너 안참
void UNS_StatusComponent::UpdateStamina(float DeltaTime)
{
	if (ANS_PlayerCharacterBase* PlayChar = Cast<ANS_PlayerCharacterBase>(GetOwner()))
	{
		float ChangingStaminaValue; // 스태미너를 변경할 값
		// 이동을 확인하기 위해 수평 속도의 절대값 합계를 계산
		float SumVelocity = FMath::Abs(PlayChar->GetMovementComponent()->Velocity.X) + FMath::Abs(PlayChar->GetMovementComponent()->Velocity.Y);


		// 점프 중이면 스태미너 변화가 없음
		if (!PlayChar->GetMovementComponent()->IsMovingOnGround())
		{
			ChangingStaminaValue = 0.f;
		}
		// 스프린트 중이고 실제로 움직이고 있다면 스태미너를 감소
		else if (PlayChar->IsSprint && !FMath::IsNearlyZero(SumVelocity))
		{
			ChangingStaminaValue = StaminaDereaseRate * DeltaTime;
		}
		// 그 외 서 있거나 걷고 있을 때스태미너를 재생
		else
		{
			// 스프린트 중이 아닐 때 재생 속도를 적용합니다.
			ChangingStaminaValue = CurrentStaminaRegenRate * DeltaTime;
		}

		AddStamina(ChangingStaminaValue); 

		// 스프린트가 비활성화된 상태에서 스태미너가 10보다 크면 다시 활성화
		if (bEnableSprint == false && Stamina > 10.f) // 변경된 조건
		{
			bEnableSprint = true;
		}

		// 스프린트 중 스태미너가 0 이하로 떨어지면 스프린트를 비활성화
		if (PlayChar->IsSprint && Stamina <= 0.f)
		{
			bEnableSprint = false;
		}
	}
}

// 체력 증감 처리
void UNS_StatusComponent::AddHealthGauge(float Value)
{
    Health = FMath::Clamp(Health + Value, 0.f, MaxHealth); // 체력을 0과 MaxHealth 사이로 고정
}

// 스탯 변경 함수 ===============================================
void UNS_StatusComponent::AddStamina(float Value)
{
    Stamina = FMath::Clamp(Stamina + Value, 0.f, MaxStamina); // 스태미너를 추가하거나 빼며, 0과 MaxStamina 사이로 고정
}

// 주어진 배율로 현재 스태미너 재생 속도를 변경합니다.
void UNS_StatusComponent::AddStaminaRegenRate(float Value)
{
	CurrentStaminaRegenRate = DefalutStaminaRegenRate * Value; // 기본 재생 속도에 배율을 적용
}

// 현재 스프린트가 활성화되어 있는지 반환합니다.
bool UNS_StatusComponent::CheckEnableSprint()
{
	return bEnableSprint;
}
//================================================================