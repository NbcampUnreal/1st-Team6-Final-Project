#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Interaction/Component/InteractionComponent.h"
#include "NS_PlayerCharacterBase.generated.h"

class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class UNS_DebugStatusWidget;  // 디버그용 위젯 차후 삭제해야함
class UNS_StatusComponent;
class UInventoryComponent;
class UNS_EquipedWeaponComponent;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_PlayerCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ANS_PlayerCharacterBase();

	FORCEINLINE UInventoryComponent* GetInventory() const { return PlayerInventory; };

	UInteractionComponent* GetInteractionComponent() const { return InteractionComponent; }
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// 피격
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

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

	
	////////////////////////////////////캐릭터 부착 컴포넌트들///////////////////////////////////////
	// 스탯 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UNS_StatusComponent* StatusComp;

	// 인터렉션 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	UInteractionComponent* InteractionComponent;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	UInventoryComponent* PlayerInventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNS_EquipedWeaponComponent* EquipedWeaponComp;

	
	////////////////////////////////////캐릭터 부착 컴포넌트들 끝!///////////////////////////////////////
	

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

	/////////////////////////////// 리플리케이션용 변수들////////////////////////////////
	// 캐릭터가 바라보고있는 좌/우 값
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Replicated Variables")
	float CamYaw;
	// 캐릭터가 바라보고있는 상/하 값
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Replicated Variables")
	float CamPitch;
	
	// 달리고있는 상태인지 확인 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Replicated Variables")
	bool IsSprint = false;
	// 발차기 확인 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Replicated Variables")
	bool IsKick = false;
	// 공격중인지 확인 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Replicated Variables")
	bool IsAttack = false;
	// 아이템을 줍고있는지 확인 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Replicated Variables")
	bool IsPickUp = false;
	// ---------------------------------------차후에 지워야함
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Replicated Variables")
	int32 IsChange = 0;
	// 캐릭터가 맞고있는지 확인 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Replicated Variables")
	bool IsHit = false;
	// 조준중인지 확인 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Replicated Variables")
	bool IsAiming = false;
	// 장전중인지 확인 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Replicated Variables")
	bool IsReload = false;
	//////////////////////////////////////////////////////////////////////////////////////
	
	
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* InputAttackAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* InputPickUpAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* InputAimingAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* InputReloadAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* InteractAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* ToggleMenuAction;
	
	// 이동 입력 잠금 제어 함수 
	UFUNCTION(BlueprintCallable, Server, Reliable, Category="Input")
	void SetMovementLockState_Server(bool bLock);
	UFUNCTION(NetMulticast, Reliable)
	void SetMovementLockState_Multicast(bool bLock);
	
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
	UFUNCTION(Server, Reliable)
	void StopSprint_Server(const FInputActionValue& Value);
	
	// 발차기
	UFUNCTION(Server, Reliable)
	void KickAction_Server(const FInputActionValue& Value);

	// 공격
	UFUNCTION(Server, Reliable)
	void StartAttackAction_Server(const FInputActionValue& Value);
	UFUNCTION(Server, Reliable)
	void StopAttackAction_Server(const FInputActionValue& Value);

	// 아이템 줍기
	UFUNCTION(Server, Reliable)
	void PickUpAction_Server(const FInputActionValue& Value);

	// 조준 
	UFUNCTION(Server, Reliable)
	void StartAimingAction_Server(const FInputActionValue& Value);
	UFUNCTION(Server, Reliable)
	void StopAimingAction_Server(const FInputActionValue& Value);

	// 재장전
	UFUNCTION(Server, Reliable)
	void ReloadAction_Server(const FInputActionValue& Value);
	//////////////////////////////////액션 처리 함수들 끝!///////////////////////////////////
	
	// 캐릭터 죽는 애니메이션 멀티캐스트
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void PlayDeath_Server();
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void PlayDeath_Multicast();

	// 카메라 Yaw값, Pitch값 서버로 전송
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void UpdateAim_Server(float NewAimYaw, float NewAimPitch);

	UFUNCTION()
	void SwapWeapon(TSubclassOf<ANS_BaseMeleeWeapon> WeaponClass);
};
