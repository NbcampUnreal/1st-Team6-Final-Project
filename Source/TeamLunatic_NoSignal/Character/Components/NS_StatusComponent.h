#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NS_StatusComponent.generated.h"

class ANS_PlayerCharacterBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAMLUNATIC_NOSIGNAL_API UNS_StatusComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UNS_StatusComponent();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime,
                               ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;

public:
    // --- Health & Stamina ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Status|Health")
    float MaxHealth = 100.f;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Status|Health")
    float Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status|Stamina")
    float MaxStamina = 100.f;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Status|Stamina")
    float Stamina;

    // --- Needs (0~100) ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Status|Needs")
    float MaxHunger  = 100.f;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Status|Needs")
    float Hunger;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Status|Needs")
    float MaxThirst  = 100.f;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Status|Needs")
    float Thirst;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Status|Needs")
    float MaxFatigue = 100.f;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Status|Needs")
    float Fatigue;

    // --- 감소/소모 설정 ---
    UPROPERTY(EditDefaultsOnly, Category="Status|Config", meta=(ClampMin="0"))
    float HungerDecreasePerMinute = 3.f;
    UPROPERTY(EditDefaultsOnly, Category="Status|Config", meta=(ClampMin="0"))
    float ThirstDecreasePerMinute = 4.f;
    UPROPERTY(EditDefaultsOnly, Category="Status|Config", meta=(ClampMin="0"))
    float FatigueDecreasePerMinute = 2.f;
    // Sprint 중일 때 Stamina 감소/재생 속도
    UPROPERTY(EditDefaultsOnly, Category="Status|Config", meta=(ClampMin="0"))
    float SprintStaminaDecreasePerSecond = 10.f;

private:
    // 기본 Tick 내부 로직
    void UpdateAllStatus(float DeltaTime);
    void ApplyStageEffects(float DeltaTime);

    void UpdateMaxStamina();

    // Clamp 처리용
    void DecreaseHealth(float Delta);
    void DecreaseStamina(float Delta);
    void DecreaseHunger(float Delta);
    void DecreaseThirst(float Delta);
    void DecreaseFatigue(float Delta);
};
