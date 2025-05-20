#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NS_ItemDataStruct.h"
#include "Engine/DataTable.h"
#include "NS_BaseWeapon.generated.h"

class ANS_PlayerCharacterBase;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_BaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	ANS_BaseWeapon();

	virtual void Attack();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipcheck")
	bool isequip;

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	UDataTable* ItemsDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	FName WeaponDataRowName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	int32 Weapondamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	float Durability;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	float Current_Durability;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	USoundBase* EquipSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	USoundBase* UnEquipSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	USoundBase* AttackSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	UStaticMesh* StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	USkeletalMesh* SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	FName AttachSocketName;

	UPROPERTY(EditAnywhere, Category = "WeaponData")
	FItemTextData ItemTextData;

	UPROPERTY(EditAnywhere, Category = "WeaponData")
	FItemNumericData ItemNumericData;

	UPROPERTY(EditAnywhere, Category = "WeaponData")
	FItemAssetData ItemAssetData;

	UFUNCTION(BlueprintCallable, Category = "WeaponData")
	float GetItemSigleWeight() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	float ItemSingleWeight = 1.0f;

	UFUNCTION(BlueprintCallable, Category = "WeaponData")
	void SetQuantity(int32 NewQuantity);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 Quantity;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	//UStaticMeshComponent* 

public:	
	virtual void Tick(float DeltaTime) override;


};
