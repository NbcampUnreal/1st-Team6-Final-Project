#pragma once

#include "CoreMinimal.h"
#include "Item/NS_BaseItem.h"
#include "NS_EAmmoType.h"
#include "NS_BaseMagazine.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_BaseMagazine : public ANS_BaseItem
{
	GENERATED_BODY()
	
public:
	ANS_BaseMagazine();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	int32 MaxAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	int32 CurrentAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	EAmmoType AmmoType;

public:
	int32 GetMaxAmmo() const { return MaxAmmo; }
	int32 GetCurrentAmmo() const { return CurrentAmmo; }
	EAmmoType GetAmmoType() const { return AmmoType; }

	void AddCurrentAmmo(int32 AddAmount);
	void MinusCurrentAmmo(int32 SubAmount);

	//UFUNCTION(BlueprintCallable, Category = "Magazine")
	//bool TryAddAmmo();
};
