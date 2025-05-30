#pragma once

#include "CoreMinimal.h"
#include "Item/NS_BaseWeapon.h"
#include "NS_EAmmoType.h"
#include "NS_BaseAmmo.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_BaseAmmo : public ANS_BaseWeapon
{
	GENERATED_BODY()
	
public:
	ANS_BaseAmmo();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	EAmmoType AmmoType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	int32 MaxAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	int32 CurrentAmmo;

public:
	int32 GetCurrentAmmo() const { return CurrentAmmo; }
	EAmmoType GetAmmoType() const { return AmmoType; }

	void AddCurrentAmmo(int32 AddAmount);
	void RemoveCurrentAmmo(int32 RemoveAmount);
};
