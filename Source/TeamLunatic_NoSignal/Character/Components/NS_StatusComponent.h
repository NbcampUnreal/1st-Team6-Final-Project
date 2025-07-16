#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "NS_StatusComponent.generated.h"

class ANS_PlayerCharacterBase;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamaged, float, DamageAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, int32, CurrentHealth, int32, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStaminaChanged, int32, CurrentStamina, int32, MaxStamina);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAMLUNATIC_NOSIGNAL_API UNS_StatusComponent : public UActorComponent
{
    GENERATED_BODY()
	
    UPROPERTY()
    TObjectPtr<ANS_PlayerCharacterBase> PlayerCharacter; // 소유하고 있는 플레이어 캐릭터에 대한 참조
    
    UNS_StatusComponent();
protected:
    virtual void BeginPlay() override;
	
	FTimerHandle StaminaTimerHandle;

	void UpdateStamina();

	// 스프린트 상태를 복제하도록 변경
	UPROPERTY(Replicated)
	bool bIsSprinting;

    // 네트워크 복제 설정
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // --- 체력 및 스태미너 ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category="Status|Health")
    int32 MaxHealth = 100.f; // 캐릭터의 최대 체력
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category="Status|Health")
    int32 Health; // 캐릭터의 현재 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Status|Stamina")
    int32 MaxStamina = 100.f; // 캐릭터의 최대 스태미너
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category="Status|Stamina")
    int32 Stamina; // 캐릭터의 현재 스태미너
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status|Stamina")
	int32 DefalutStaminaRegenRate = 10.f; // 스태미너 기본 재생 속도
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status|Stamina")
	int32 CurrentStaminaRegenRate; // 현재 스태미너 재생 속도 (버프/디버프에 의해 변경될 수 있음)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status|Stamina")
    int32 StaminaDereaseRate = -10.f; // 스프린트 시 스태미너 감소 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SprintMultifly")
	float SprintMultiply = 1.5f;// 캐릭터 달리기 배율 속도

public:
    // 스탯 값 변경을 위한 함수
    void UpdateHealthChange(float Value);
    void UpdateStaminaChange(float Value);

	void StartSprinting();
	void StopSprinting();
    
    // 델리게이트
    UPROPERTY(BlueprintAssignable)
    FOnDamaged OnDamaged;

    UPROPERTY(BlueprintAssignable)
    FOnHealthChanged OnHealthChanged;
    
    UPROPERTY(BlueprintAssignable)
    FOnStaminaChanged OnStaminaChanged;

    int32 GetCurrentHealth() const { return Health; }
    int32 GetCurrentStmina() const { return Stamina; }

private:
    bool EnableSprint = true; // 스프린트 허용 여부를 결정하는 플래그
};