#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NS_StatusComponent.generated.h"

class ANS_PlayerCharacterBase;



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAMLUNATIC_NOSIGNAL_API UNS_StatusComponent : public UActorComponent
{
    GENERATED_BODY()

private:
    UPROPERTY()
    TObjectPtr<ANS_PlayerCharacterBase> PlayerCharacter;

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
	float DefalutStaminaRegenRate = 10.f; // 스태미너 재생 속도
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status|Stamina")
	float CurrentStaminaRegenRate; // 현재 스태미너 재생 속도
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status|Stamina")
    float StaminaDereaseRate = 10.f;



    //스탯 수치 변경용
    void AddHealthGauge(float Value);
    void AddStamina(float Value);
    void AddStaminaRegenRate(float Value);

private:


    //틱 내부에서 업데이트하는 함수 모음
    void UpdateAllStatus(float DeltaTime);

    void UpdateStamina(float DeltaTime);
    void UpdateMaxStamina();

};
