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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY()
	TObjectPtr<ANS_PlayerCharacterBase> PlayerCharacter; // 소유하고 있는 플레이어 캐릭터에 대한 참조

	FTimerHandle StaminaTimerHandle;
	
	void UpdateStamina();
	
	bool EnableSprint = true; // 스프린트 허용 여부를 결정하는 플래그

protected:
	// --- 체력(Health) 관련 ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = "Status|Health")
	int32 MaxHealth = 100;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category="Status|Health")
	int32 CurrentHealth;

	// --- 스태미나(Stamina) 관련 ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = "Status|Stamina")
	int32 MaxStamina = 100;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category="Status|Stamina")
	int32 CurrentStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status|Stamina")
	int32 DefaultStaminaRegenRate = 10;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status|Stamina")
	int32 CurrentStaminaRegenRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status|Stamina")
	int32 StaminaDereaseRate = -10;

	// --- 달리기(Sprint) 관련 ---
	UPROPERTY(ReplicatedUsing = OnRep_IsSprinting)
	bool bIsSprinting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status|Sprint")
	float SprintMultiply = 1.5f;

	UFUNCTION()
	void OnRep_IsSprinting();

	UFUNCTION(Server, Reliable)
	void Server_SetSprinting(bool bShouldSprint);

public:
	void StartSprinting();
	void StopSprinting();
	void SetMovementSpeed(bool bSprinting);

	UFUNCTION(BlueprintPure)
	int32 GetCurrentHealth() const { return CurrentHealth; }
	
	void UpdateHealthChange(float Value);
	
	UFUNCTION(BlueprintPure)
	int32 GetCurrentStamina() const { return CurrentStamina; }
	
	void UpdateStaminaChange(float Value);
};
