#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Interaction/Component/InteractionComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SpotLightComponent.h"
#include "GameFlow/NS_GameModeBase.h"
#include "GameFlow/NS_MainGamePlayerState.h"
#include "Character/ThrowActor/NS_ThrowActor.h"
#include "UI/NS_OpenLevelMap.h"
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
class UNS_PlayerController;
class UNS_OpenLevelMap;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_PlayerCharacterBase : public ACharacter
{
	GENERATED_BODY()
	
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


// =========================================퀵슬롯 관련 변수 및 함수들=================================================
	// 퀵슬롯 선택 함수들
	UFUNCTION()
	void QuickSlot1Selected();

	UFUNCTION()
	void QuickSlot2Selected();

	UFUNCTION()
	void QuickSlot3Selected();

	UFUNCTION()
	void QuickSlot4Selected();

	UFUNCTION()
	void QuickSlot5Selected();

	// 키 입력에 따른 퀵슬롯 선택
	UFUNCTION(BlueprintCallable, Category = "QuickSlot")
	void HandleQuickSlotKeyInput(int32 KeyNumber);

	// 서버에 퀵슬롯 사용 요청
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_UseQuickSlotByIndex(int32 Index);

	// 모든 클라이언트에 퀵슬롯 사용 알림
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void Multicast_UseQuickSlotByIndex(int32 Index);

	// 퀵슬롯 컴포넌트에 접근
	UFUNCTION(BlueprintCallable, Category = "QuickSlot")
	UNS_QuickSlotComponent* GetQuickSlotComponent() const { return QuickSlotComponent; }

	// 현재 퀵슬롯 인덱스로 1번 슬롯부터 시작
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "QuickSlot", meta = (AllowPrivateAccess = "true"))
	int32 CurrentQuickSlotIndex = 0;

	// 현재 선택된 퀵슬롯 인덱스 반환 ====== 노티파이에서 호출
	UFUNCTION(BlueprintCallable, Category = "QuickSlot")
	int32 GetCurrentQuickSlotIndex() const { return CurrentQuickSlotIndex; }

	// 아이템 획득 시 자동으로 퀵슬롯에 할당하고 장착 애니메이션 실행
	UFUNCTION(BlueprintCallable, Category = "Inventory|QuickSlot")
	void AutoEquipPickedUpItem(UNS_InventoryBaseItem* NewItem);
// ===============================================================================================================================
	
	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
	UNS_InventoryBaseItem* AssignedItem;

	UFUNCTION(Client, Reliable)
	void Client_NotifyInventoryUpdated();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_HideTipText();

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

	// 헤드램프 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flashlight")
	USpotLightComponent* FlashlightComponent;
	

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
	////////////////////////////////////////병투척 변수 끝!///////////////////////////////////////////////


	// 죽음 애니메이션 몽타주
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* DeathMontage;


	// LookAction에 카메라 회전값 보간 속도 ---> 8은 너무 느려서 10이상은 되어야할 듯
	UPROPERTY(EditDefaultsOnly, Category = "Aim")
	float AimSendInterpSpeed = 10.f;

	// 점프가 가능하게 하는 변수 
	bool IsCanJump = true;
	
	// 점프 타이머 핸들
	FTimerHandle JumpTimerHandle;
	// =================================================================================================
	
	// ==================================== 데미지 받을 때 재생할 사운드 =========================================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* DamageSound;
	// =================================================================================================

	
	// =================================Turn In Place관련 변수들 ===============================
	// Turn In Place가 가능한 Yaw회전 값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float TurnInPlaceThreshold = 90.0f; 
	// 몸 회전 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float TurnInPlaceSpeed = 5.0f;   
	// 몸 회전이 완료된 후 Yaw값을 0으로 리셋하는 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float TurnInPlaceResetThreshold = 10.0f;

	// 현재 회전할 때 Yaw값
	float CurrentTurnYaw = 0.0f;
	// 현재 회전 중인지 여부
	bool bIsTurningInPlace = false;
	// CamYaw를 0으로 보간하는 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float TurnInPlaceYawResetSpeed = 10.0f;
	// 회전 후 Yaw 값을 리셋 중인지 여부
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	bool bIsResettingYaw = false;
	// 마지막 회전 Yaw 값
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	float LastTurnYaw = 0.0f;
	// ===============================Turn In Place변수 끝!===================================


	
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
	// 헤드램프 키고 끄는 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Replicated Variables")
	bool bFlashlightOnOff = true;
	// 무기 교체중인지 확인 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Replicated Variables")
	bool IsChangingWeapon = false;
	// 퀵슬롯을 누르면 퀵슬롯에 있는 무기를 장착하는 애니메이션 재생용 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Replicated Variables")
	bool IsChangeAnim = false;
	// 캐릭터가 죽었는지 확인 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Replicated Variables")
	bool IsDead = false;
	// 한번만 던져지도록 실행하는 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Replicated Variables")
	bool bHasThrown = false;
	// 병 던질 수 있는지 확인 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Replicated Variables")
	bool IsThrow = false;
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* InputFlashlightAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* InputOpenMapAction;
	//퀵슬롯 바인딩
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* InputQuickSlot1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* InputQuickSlot2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* InputQuickSlot3;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* InputQuickSlot4;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* InputQuickSlot5;
	
	

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
	void StartSprint(const FInputActionValue& Value);
	void StopSprint(const FInputActionValue& Value);
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
	//////////////////////////////////액션 처리 함수들 끝!///////////////////////////////////

	// 데미지 받으면 모든 클라이언트에 멀티캐스트
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_TakeDmage(float DamageAmount);

	
	// 캐릭터 죽는 애니메이션 멀티캐스트
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void PlayDeath_Server();
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void PlayDeath_Multicast();

	// 카메라 Yaw값, Pitch값 서버로 전송
	UFUNCTION(BlueprintCallable, Server, Unreliable)
	void UpdateAim_Server(float NewCamYaw, float NewCamPitch);
	
	// 현재 캐릭터가 바라보는 카메라 Yaw값 업데이트 함수
	UFUNCTION(NetMulticast, Unreliable)
	void UpdateAim_Multicast(float Yaw, float Pitch);
	
	// 헤드램프 켜고 끄는 함수
	UFUNCTION(BlueprintCallable, Category = "Flashlight")
	void ToggleFlashlight();

	// 헤드램프 켜고 끄는 서버 전송 함수
	UFUNCTION(Server, Reliable)
	void ToggleFlashlight_Server();
	// 헤드램프 켜고 끄는 멀티캐스트 전송 함수
	UFUNCTION(NetMulticast, Reliable)
	void ToggleFlashlight_Multicast();

	// 캐릭터가 병투척해서 날아가는 속도/방향/궤도 함수
	UFUNCTION(BlueprintCallable)
	void ThrowBottle();
	
	// ======================== Turn In Place 관련 함수 =================================
	// Turn In Place 업데이트 함수
	void UpdateTurnInPlace(float DeltaTime);
	
	// TurnLeft/Right 변수값 false로 만들고 CamYaw값 0으로 부드럽게 보간할 함수 (노티파이에서 호출할 함수임)
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void OnTurnInPlaceFinished();

	// Turn In Place 상태를 서버에 업데이트하는 함수
	UFUNCTION(Server, unreliable)
	void Server_UpdateTurnInPlaceState(bool bInTurnLeft, bool bInTurnRight, bool bInUseControllerDesiredRotation);
	// Turn In Place 상태를 모든 클라이언트에 멀티캐스트하는 함수
	UFUNCTION(NetMulticast, unreliable)
	void Multicast_UpdateTurnInPlaceState(bool bInTurnLeft, bool bInTurnRight, bool bInUseControllerDesiredRotation);

	// Yaw 리셋 관련 함수
	void UpdateYawReset(float DeltaTime);
	// ======================== Turn In Place 끝! =================================

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OpenLevelMap")
	TSubclassOf<UNS_OpenLevelMap> OpenLevelMapWidgetClass;
	
	UPROPERTY()
	UNS_OpenLevelMap* CurrentOpenMapWidget;

	void OpenMapAction(const FInputActionValue& Value);
	
	//환각 관련
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PostProcess")
	UMaterialInterface* HallucinationMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PostProcess")
	UMaterialInstanceDynamic* HallucinationMID;
	
	// 환각효과 켜기
	void ActivateHallucinationEffect(float Duration);

	// 사운드 멀티캐스트
	UFUNCTION(NetMulticast, Reliable)
	void PlaySoundOnCharacter_Multicast(USoundBase* SoundToPlay);
	
	// 캐릭터가 현재 조준 중인지 확인하는 함수
	UFUNCTION(BlueprintCallable, Category = "Character")
	bool IsAimingChange() const { return IsAiming; }
};
