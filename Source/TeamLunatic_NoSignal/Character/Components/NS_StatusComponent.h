#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NS_StatusComponent.generated.h"

class ANS_PlayerCharacterBase; // 플레이어 캐릭터 기본 클래스의 전방 선언



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAMLUNATIC_NOSIGNAL_API UNS_StatusComponent : public UActorComponent
{
    GENERATED_BODY()

private:
    UPROPERTY()
    TObjectPtr<ANS_PlayerCharacterBase> PlayerCharacter; // 소유하고 있는 플레이어 캐릭터에 대한 참조

public:
    // 이 컴포넌트의 기본 속성들을 설정합니다.
    UNS_StatusComponent();

protected:
    // 게임이 시작될 때 호출됩니다.
    virtual void BeginPlay() override;
    // 매 프레임마다 호출됩니다.
    virtual void TickComponent(float DeltaTime,
                               ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;

public:
    // --- 체력 및 스태미너 ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Status|Health")
    float MaxHealth = 100.f; // 캐릭터의 최대 체력
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Status|Health")
    float Health; // 캐릭터의 현재 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status|Stamina")
    float MaxStamina = 100.f; // 캐릭터의 최대 스태미너
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Status|Stamina")
    float Stamina; // 캐릭터의 현재 스태미너
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status|Stamina")
	float DefalutStaminaRegenRate = 10.f; // 스태미너 기본 재생 속도
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status|Stamina")
	float CurrentStaminaRegenRate; // 현재 스태미너 재생 속도 (버프/디버프에 의해 변경될 수 있음)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status|Stamina")
    float StaminaDereaseRate = -20.f; // 스프린트 시 스태미너 감소 속도



    // 스탯 값 변경을 위한 함수
    void AddHealthGauge(float Value); // 체력을 증가 또는 감소시킵니다.
    void AddStamina(float Value); // 스태미너를 증가 또는 감소시킵니다.
    void AddStaminaRegenRate(float Value); // 스태미너 재생 속도를 변경합니다.
    bool CheckEnableSprint(); // 현재 스프린트가 가능한지 확인합니다.

private:
    bool bEnableSprint = true; // 스프린트 허용 여부를 결정하는 플래그

    // 틱 내부에서 업데이트되는 함수 모음
    void UpdateAllStatus(float DeltaTime); // (현재 구현되지 않았거나 사용되지 않지만, 전체 상태 업데이트를 위해 의도된 것으로 보임)

    void UpdateStamina(float DeltaTime); // 행동에 따라 스태미너를 업데이트합니다.
    // void UpdateMaxStamina(); // 제거됨: 현재 체력에 따라 최대 스태미너를 업데이트하는 함수

};