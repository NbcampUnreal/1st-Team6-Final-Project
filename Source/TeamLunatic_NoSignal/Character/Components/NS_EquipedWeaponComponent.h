#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/NS_WeaponType.h"
#include "NS_EquipedWeaponComponent.generated.h"

class ANS_PlayerCharacterBase;
class ANS_BaseWeapon; 
class UNS_InventoryBaseItem;
  
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAMLUNATIC_NOSIGNAL_API UNS_EquipedWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

	
	UNS_EquipedWeaponComponent();
	
	virtual void BeginPlay() override;

public:
	///////////////////////////////////////리플리케이션 변수//////////////////////////////////////////
	// 공격중인지 확인 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Replicated Variables")
	bool IsAttack = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Replicated Variables")
	bool IsEmpty = false;

	// 현재 장착중인 무기 타입 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category="Replicated Variables")
	EWeaponType WeaponType = EWeaponType::Unarmed;

	// 현재 원거리무기에 사격 모드 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Replicated Variables")
	ERangeChangeFireMode CurrentFireMode;
	
	// 현재 장착 중인 무기 액터 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category="Replicated Variables")
	ANS_BaseWeapon* CurrentWeapon;
	///////////////////////////////////////////////////////////////////////////////////////////

	// 서버-클라이언트 복제 설정
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** 슬롯 인덱스에 해당하는 무기를 장착 */
	UFUNCTION(BlueprintCallable)
	void SwapWeapon(TSubclassOf<ANS_BaseWeapon> WeaponClass, UNS_InventoryBaseItem* SourceItem);
	
	// 서버에서 무기 스폰 및 부착 함수
	UFUNCTION(Server, Reliable)
	void ServerEquipWeapon(TSubclassOf<ANS_BaseWeapon> WeaponClass, UNS_InventoryBaseItem* SourceItem);
	// 클라이언트에 무기 스폰 및 부착 함수 
	UFUNCTION(NetMulticast, Reliable)
	void MulticastEquipWeapon(TSubclassOf<ANS_BaseWeapon> WeaponClass, UNS_InventoryBaseItem* SourceItem);
	UFUNCTION(BlueprintCallable)
	void UnequipWeapon();
	
	UFUNCTION(Server, Reliable)
	void Server_UnequipWeapon();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UnequipWeapon();

	UNS_InventoryBaseItem* GetCurrentWeaponItem() const;
	// 현재 장착 무기가 원기리일때는 재장전 
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_Reload();
	// 재장전 멀티캐스트
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Reload();

	
	// 현재 플레이하는 캐릭터 변수
	UPROPERTY()
	ANS_PlayerCharacterBase* OwnerCharacter;
};
