#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NS_StatusComponent.generated.h"

class ANS_PlayerCharacterBase;

UENUM(BlueprintType)
enum class EHungerStatus : uint8
{
	Full,      // 배부름
	Normal,    // 정상
	LittleHungry, // 약간 배고픔
	Hungry,    // 배고픔
	Starving   // 굶주림
};

UENUM(BlueprintType)
enum class EThirstStatus : uint8
{
	Full,      // 충분함
	Normal,    // 정상
	LittleThirsty, // 약간 목마름
	Thirsty,   // 목마름
	Dehydrated // 탈수 상태
};

UENUM(BlueprintType)
enum class EFatigueStatus : uint8
{
	Rested,    // 충분한 휴식
	Normal,    // 정상
	Tired,     // 피곤함
	Exhausted, // 탈진 상태
    Unconscious    // 기절 상태
};

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
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Status|Health")
    float Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status|Stamina")
    float MaxStamina = 100.f;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Status|Stamina")
    float Stamina;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status|Stamina")
	float StaminaRegenRate = 10.f; // 스태미너 재생 속도
    // Sprint 중일 때 Stamina 감소/재생 속도
    UPROPERTY(EditDefaultsOnly, Category = "Status|Stamina", meta = (ClampMin = "0"))
    float SprintStaminaDecreasePerSecond = 10.f;

    // --- Needs (0~100) ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Status|Needs|Hunger")
    float MaxHunger  = 100.f;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Status|Needs|Hunger")
    float CurrentHunger;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status|Needs|Hunger")
	EHungerStatus HungerStatus = EHungerStatus::Full;
    UPROPERTY(EditDefaultsOnly, Category = "Status|Needs|Hunger", meta = (ClampMin = "0"))
    float HungerDecreasePerMinute = 3.f;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Status|Needs|Thirst")
    float MaxThirst  = 100.f;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Status|Needs|Thirst")
    float CurrentThirst;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status|Needs|Thirst")
	EThirstStatus ThirstStatus = EThirstStatus::Full;
    UPROPERTY(EditDefaultsOnly, Category = "Status|Needs|Thirst", meta = (ClampMin = "0"))
    float ThirstDecreasePerMinute = 4.f;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Status|Needs|Fatigue")
    float MaxFatigue = 100.f;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Status|Needs|Fatigue")
    float CurrentFatigue;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status|Needs|Fatigue")
	EFatigueStatus FatigueStatus = EFatigueStatus::Rested;
    UPROPERTY(EditDefaultsOnly, Category="Status|Needs|Fatigue", meta=(ClampMin="0"))
    float FatigueDecreasePerMinute = 2.f;

    //스탯 수치 변경용
    void AddHealthGauge(float Value);
    void AddStamina(float Value);
    void AddHunger(float Value);
    void AddThirst(float Value);
    void AddFatigue(float Value);


private:
    //틱 내부에서 업데이트하는 함수 모음
    void UpdateAllStatus(float DeltaTime);

    //스탯 enum관련
    void UpdateStatusEnums();
	void ChangeStatusDueHunger(EHungerStatus NewStatus);
	void ChangeStatusDueThirst(EThirstStatus NewStatus);
	void ChangeStatusDueFatigue(EFatigueStatus NewStatus);

    void UpdateStamina(float DeltaTime);
    void UpdateMaxStamina();

};
