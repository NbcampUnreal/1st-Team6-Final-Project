#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Character/Components/NS_StatusComponent.h"
#include "NS_PlayerCharacterBase.generated.h"

class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class UNS_DebugStatusWidget;  // 디버그용 위젯 차후 삭제해야함
class UNS_StatusComponent;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_PlayerCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ANS_PlayerCharacterBase();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//피격
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	void OnDeath();


public:
	// 카메라를 붙일 소켓 이름 [에디터에서 변경 가능함] 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	FName CameraAttachSocketName = TEXT("head");

	// 1인칭 카메라 컴포넌트 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	
	// ============== 디버그용 위젯 차후 삭제해야 함 ===================
	// 에디터에서 할당할 위젯 Blueprint 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UNS_DebugStatusWidget> DebugWidgetClass;

	
	// 런타임에 생성될 위젯 ============= 차후 삭제 필요
	UPROPERTY()
	UNS_DebugStatusWidget* DebugWidgetInstance;

	
	// 스탯 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UNS_StatusComponent* StatusComp;

	
	// 캐릭터 이동 중 바라보는 곳으로 몸 회전 속도 (1 ~ 10까지 해봤는데 5가 가장 적당함)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CharacterTurnSpeed = 5.0f;
	
	
	// ========== 이동 관련 =============
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float DefaultWalkSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeedMultiplier;
	// 점프가 가능하게 하는 변수 
	bool IsCanJump = true;

	// 달리고있는 상태확인 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Sprint")
	bool IsSprint = false;
	// =========== 발차기 확인 변수 =============
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Kick")
	bool IsKick = false;

	
	// IMC(입력 매핑 컨텍스트)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	// IA(입력 액션들) 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* InputMoveAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* InputLookAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* InputJumpAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* InputCrouchAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* InputSprintAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* InputKickAction;

	// 이동 입력 잠금 제어 함수 
	UFUNCTION(BlueprintCallable, Category="Input")
	void SetMovementLockState(bool bLock);
	
	//////////////////////////////////액션 처리 함수들///////////////////////////////////
	//////////////CharacterMovmentComponent를 사용함////////////////
	// 이동
	void MoveAction(const FInputActionValue& Value);
	// 마우스 카메라 
	void LookAction(const FInputActionValue& Value);
	// 점프
	void JumpAction(const FInputActionValue& Value);
	// 앉기
	void StartCrouch(const FInputActionValue& Value);
	void StopCrouch(const FInputActionValue& Value);
	//////////////CharacterMovmentComponent를 사용안함////////////////
	// 달리기
	UFUNCTION(server, Reliable)
	void StartSprint_Server(const FInputActionValue& Value);
	UFUNCTION(NetMulticast, Reliable)
	void StartSprint_Multicast();

	UFUNCTION(Server, Reliable)
	void StopSprint_Server(const FInputActionValue& Value);
	UFUNCTION(NetMulticast, Reliable)
	void StopSprint_Multicast();

	// 발차기
	UFUNCTION(Server, Reliable)
	void KickAction_Server(const FInputActionValue& Value);
	UFUNCTION(NetMulticast, Reliable)
	void KickAction_Multicast();
	//////////////////////////////////액션 처리 함수들 끝!///////////////////////////////////
};
