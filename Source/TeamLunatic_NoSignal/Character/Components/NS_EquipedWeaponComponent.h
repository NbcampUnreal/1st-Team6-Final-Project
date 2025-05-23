#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NS_EquipedWeaponComponent.generated.h"

class ANS_PlayerCharacterBase;
class ANS_BaseWeapon; 
  
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAMLUNATIC_NOSIGNAL_API UNS_EquipedWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNS_EquipedWeaponComponent();

	// 서버-클라이언트 복제 설정
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** 슬롯 인덱스에 해당하는 무기를 장착 */
	UFUNCTION(BlueprintCallable)
	void SwapWeapon(TSubclassOf<ANS_BaseMeleeWeapon> WeaponClass);
	
	// 서버에서 무기 스폰 및 부착 함수
	UFUNCTION(Server, Reliable)
	void ServerEquipWeapon(TSubclassOf<ANS_BaseMeleeWeapon> WeaponClass);
	// 클라이언트에 무기 스폰 및 부착 함수 
	UFUNCTION(NetMulticast, Reliable)
	void MulticastEquipWeapon(TSubclassOf<ANS_BaseMeleeWeapon> WeaponClass);

	// 현재 장착 무기로 발사 
	UFUNCTION(BlueprintCallable)
	void Fire();

	// 현재 장착 무기가 원기리일때는 재장전 
	UFUNCTION(BlueprintCallable)
	void Reload();
	
	virtual void BeginPlay() override;

	/** 실제로 소유한 캐릭터 */
	UPROPERTY()
	ANS_PlayerCharacterBase* OwnerCharacter;

	/** 현재 장착 중인 무기 액터 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
    ANS_BaseWeapon* CurrentWeapon;

	/** 메쉬에 붙일 소켓 이름 (에디터에서 필요 시 변경) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Equip")
	FName WeaponAttachSocketName = TEXT("hand_rKnife");
};
