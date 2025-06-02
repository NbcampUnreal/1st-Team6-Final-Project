#pragma once

#include "CoreMinimal.h"
#include "Item/NS_BaseItem.h"
#include "NS_EAmmoType.h"
#include "NS_BaseMagazine.generated.h"

class ANS_BaseAmmo;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_BaseMagazine : public ANS_BaseItem
{
	GENERATED_BODY()
	
public:
	ANS_BaseMagazine();

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	int32 MaxAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	int32 CurrentAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	EAmmoType AmmoType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	TSubclassOf< ANS_BaseAmmo> AmmoClass;


	int32 GetMaxAmmo() const { return MaxAmmo; }

	UFUNCTION(BlueprintCallable, Category = "Magazine")
	int32 GetCurrentAmmo() const { return CurrentAmmo; }

	EAmmoType GetAmmoType() const { return AmmoType; }

	//빈공간 체크
	UFUNCTION(BlueprintCallable, Category = "Magazine")
	bool IsEmpty() const;

	//총알이 가득 찼는지 확인
	UFUNCTION(BlueprintCallable, Category = "Magazine")
	bool isFull() const;

	//총알 사용
	UFUNCTION(BlueprintCallable, Category = "Magazine")
	bool ConsumeOne();

	//탄환 전부 제거
	UFUNCTION(BlueprintCallable, Category = "Magazine")
	int32 ConsumeAll();

	//원하는 수 만큼 탄환 제거
	UFUNCTION(BlueprintCallable, Category = "Magazine")
	int32 RemoveAmmo(int32 Amount);

	//탄약 채우기
	UFUNCTION(BlueprintCallable, Category = "Magazine")
	bool Refill(int32 Amount, EAmmoType InputAmmoType);

	//해당 탄환 사용가능 여부 체크
	UFUNCTION(BlueprintCallable, Category = "Magazine")
	bool CanUseAmmoType(EAmmoType InputType) const;

	//남은 공간 확인
	UFUNCTION(BlueprintCallable, Category = "Magazine")
	int32 GetRemainingCapacity() const;

	TSubclassOf<ANS_BaseAmmo> GetAmmoClass() const { return AmmoClass; }
};
