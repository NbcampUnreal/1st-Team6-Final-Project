#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Interaction/Component/InteractionComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFlow/NS_GameModeBase.h"
#include "Character/ThrowActor/NS_ThrowActor.h"
#include "NS_PlayerCharacterBase.generated.h"

class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class UNS_StatusComponent;
class UNS_InventoryBaseItem;
class UInventoryComponent;
class ANS_BaseWeapon;
class UNS_EquipedWeaponComponent;
class UNS_QuickSlotPanel;
class UNS_QuickSlotComponent;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_PlayerCharacterBase : public ACharacter
{
	GENERATED_BODY()

private:

	// ========== 이동 관련 =============
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float DefaultWalkSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true", UIMin = 0))
	float SprintSpeedMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true", UIMin = 0, UIMax = 2))
	float SpeedMultiAtStat = 1.0f; //버프|디버프 때 조절될 속도 배율


	//조준이 가능한지 확인하는 변수
	bool IsAvaliableAiming = true;

public:
	ANS_PlayerCharacterBase();

	FORCEINLINE UInventoryComponent* GetInventory() const { return PlayerInventory; };

	UInteractionComponent* GetInteractionComponent() const { return InteractionComponent; }

	//캐릭터의 스피드배율 변경용
	FORCEINLINE void SetSpeedMultiply(float MultiplyValue) { SpeedMultiAtStat = MultiplyValue; };

	FORCEINLINE void SetAvailableAiming(bool bAvailable) { IsAvaliableAiming = bAvailable; };

	void DropItem(UNS_InventoryBaseItem* ItemToDrop, const int32 QuantityToDrop);

	UFUNCTION(Client, Reliable)
	void Client_NotifyQuickSlotUpdated();

	UFUNCTION(BlueprintCallable)
	void UseThrowableItem_Internal(int32 Index);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_UseThrowableItem(int32 Index);

	UFUNCTION(Server, Reliable)
	void Server_AssignQuickSlot(int32 SlotIndex, UNS_InventoryBaseItem* Item);

	void HandleUseThrowableItem(int32 Index);

	void UseQuickSlot1();

	void UseQuickSlot2();

	void UseQuickSlot3();

	UFUNCTION(BlueprintCallable)
	void UseQuickSlotByIndex(int32 Index);

	void UseQuickSlotByIndex_Internal(int32 Index);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_UseQuickSlotByIndex(int32 Index);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UseQuickSlotByIndex(int32 Index);

	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
	UNS_InventoryBaseItem* AssignedItem;

	UFUNCTION(Client, Reliable)
	void Client_NotifyInventoryUpdated();

	UFUNCTION(Server, Reliable)
	void Server_UseInventoryItem(FName ItemRowName);
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// 피격
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

public:
	////////////////////////////////////캐릭터 부착 컴포넌트들///////////////////////////////////////
	// 스프링 암 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;
	// 1인칭 카메라 컴포넌트 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;
	// 1인칭 팔 스켈레탈 메시 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FirstPerson")
	USkeletalMeshComponent* FirstPersonArms;

	// 스탯 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNS_StatusComponent* StatusComp;

	// 인터렉션 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	UInteractionComponent* InteractionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", Replicated)
	UInventoryComponent* PlayerInventory;

	UPROPERTY()
	UNS_QuickSlotPanel* QuickSlotPanel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "QuickSlot", Replicated)
	UNS_QuickSlotComponent* QuickSlotComponent;

	// 장착 무기 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNS_EquipedWeaponComponent* EquipedWeaponComp;
	////////////////////////////////////캐릭터 부착 컴포넌트들 끝!///////////////////////////////////////



	/////////////////////////////////병투척 변수 + 병이 날아갈 소켓 위치 변수 //////////////////////////////
	// 캐릭터가 던지는 병 액터 클래스변수 설정
	UPROPERTY(EditDefaultsOnly, Category = "Throw")
	TSubclassOf<class ANS_ThrowActor> BottleClass;

	// 던질 때 기준이 되는 소켓 이름 == 캐릭터 블루프린트에서 설정해주면 됨
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throw")
	FName ThrowSocketName;

	// 한번만 던져지도록 실행하는 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Replicated Variables")
	bool bHasThrown = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Replicated Variables")
	bool IsThrow = false;
	////////////////////////////////////////병투척 변수 끝!///////////////////////////////////////////////



	// LookAction에 카메라 회전값 보간 속도 ---> 8은 너무 느려서 10이상은 되어야할 듯
	UPROPERTY(EditDefaultsOnly, Category = "Aim")
	float AimSendInterpSpeed = 10.f;

	// 점프가 가능하게 하는 변수 
	bool IsCanJump = true;
	// ====================================


	/////////////////////////////// 리플리케이션용 변수들////////////////////////////////
	// 캐릭터가 바라보고있는 좌/우 값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Replicated Variables")
	float CamYaw;
	// 캐릭터가 바라보고있는 상/하 값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Replicated Variables")
	float CamPitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Look|Assist")
	float LookMagnification = 0.5f;

	// 왼쪽으로 몸을 회전시키는 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Replicated Variables")
	bool TurnLeft = false;
	// 오른쪽으로 몸을 회전시키는 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Replicated Variables")
	bool TurnRight = false;
	// 사격시 몸전체Mesh 사격 애니메이션 재생 용 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Replicated Variables")
	bool NowFire = false;
	// 달리고있는 상태인지 확인 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Replicated Variables")
	bool IsSprint = false;
	// 재장전 실행 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Replicated Variables")
	bool IsReload = false;
	// 아이템을 줍고있는지 확인 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Replicated Variables")
	bool IsPickUp = false;
	// 캐릭터가 맞고있는지 확인 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Replicated Variables")
	bool IsHit = false;
	// 조준중인지 확인 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Replicated Variables")
	bool IsAiming = false;
	// 무기 교체중인지 확인 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Replicated Variables")
	bool IsChangingWeapon = false;
	// 퀵슬롯을 누르면 퀵슬롯에 있는 무기를 장착하는 애니메이션 재생용 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_IsChangeAnim, Category = "Replicated Variables")
	bool IsChangeAnim = false;

	UFUNCTION()
	void OnRep_IsChangeAnim();
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
	//퀵슬롯 바인딩
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* InputQuickSlot1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* InputQuickSlot2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* InputQuickSlot3;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* ToggleHeadLampAction;


	// 캐릭터 EnhancedInput을 없앴다가 다시 부착하는는 함수 IMC를 지워웠다가 다시 장착하게해서 AnimNotify로 발차기 공격동안 IMC없앰
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Input")
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

	// 아이템 줍기
	UFUNCTION(Server, Reliable)
	void PickUpAction_Server(const FInputActionValue& Value);

	// 아이템 버리기
	UFUNCTION(Server, Reliable)
	void DropItem_Server(UNS_InventoryBaseItem* ItemToDrop, int32 QuantityToDrop);

	// 조준 
	UFUNCTION(Server, Reliable)
	void StartAimingAction_Server(const FInputActionValue& Value);
	UFUNCTION(Server, Reliable)
	void StopAimingAction_Server(const FInputActionValue& Value);

	// 재장전
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ReloadAction_Server(const FInputActionValue& Value);
	//////////////////////////////////액션 처리 함수들 끝!///////////////////////////////////

	// 캐릭터 죽는 애니메이션 멀티캐스트
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void PlayDeath_Server();
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void PlayDeath_Multicast();

	// 카메라 Yaw값, Pitch값 서버로 전송
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void UpdateAim_Server(float NewCamYaw, float NewCamPitch);


	// 캐릭터가 병투척해서 날아가는 속도/방향/궤도 함수
	UFUNCTION(BlueprintCallable)
	void ThrowBottle();
};
