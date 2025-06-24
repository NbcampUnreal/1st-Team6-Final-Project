#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Sound/SoundBase.h"
#include "NiagaraSystem.h"
#include "Item/NS_WeaponType.h"
#include "Item/NS_EAmmoType.h"
#include "NS_ItemDataStruct.generated.h"

class ANS_BaseWeapon;

UENUM()
enum class EItemType : uint8
{
	Misc UMETA(DisplayName = "기타"),
	Consumable UMETA(DisplayName = "소모품"),
	Medical UMETA(DisplayName = "치료품"),
	Equipment UMETA(DisplayName = "장비"),
	Material UMETA(DisplayName = "제작재료"),
	Utility UMETA(DisplayName = "도구"),
	EndingTrigger UMETA(DisplayName = "엔딩조건")
};

USTRUCT()
struct FItemTextData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	FText ItemName;

	UPROPERTY(EditAnywhere, meta = (MultiLine = "true"))
	FText ItemDescription;

	UPROPERTY(EditAnywhere)
	FText InteractionText;
};

USTRUCT()
struct FItemStates
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	float HealAmount;

	UPROPERTY(EditAnywhere)
	float StaminaRecovery;

	UPROPERTY(EditAnywhere)
	float HungerRestore;

	UPROPERTY(EditAnywhere)
	float ThirstRestore;

	UPROPERTY(EditAnywhere)
	float TiredRestore;
};

USTRUCT()
struct FItemFlags
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	bool isbleed;

	UPROPERTY(EditAnywhere)
	bool isbroken;

	UPROPERTY(EditAnywhere)
	bool isstomachache;
};

USTRUCT()
struct FItemNumericData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	float Weight;

	UPROPERTY(EditAnywhere)
	int32 MaxStack;

	UPROPERTY(EditAnywhere)
	bool isStackable;
};

USTRUCT()
struct FItemAssetData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere)
	UStaticMesh* StaticMesh;

	UPROPERTY(EditAnywhere)
	USkeletalMesh* SkeletalMesh;

	UPROPERTY(EditAnywhere)
	USoundBase* GetSound = nullptr;
};

USTRUCT()
struct FConsumAbleItemAssetData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	USoundBase* UseSound = nullptr;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* UseEffect;
};

USTRUCT()
struct FWeaponData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	int32 Damage;

	UPROPERTY(EditAnywhere)
	float Durability;

	UPROPERTY(EditAnywhere)
	int32 MaxAmmo;

	UPROPERTY(EditAnywhere)
	EAmmoType AmmoType;
};


USTRUCT(BlueprintType)
struct FNS_ItemDataStruct : public FTableRowBase
{
public:
	GENERATED_USTRUCT_BODY()

	//있어야 될 데이터
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemDataRowName;

	UPROPERTY(EditAnywhere)
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponType")
	EWeaponType  WeaponType;

	UPROPERTY(EditAnywhere)
	FItemTextData ItemTextData;

	UPROPERTY(EditAnywhere)
	FWeaponData WeaponData;

	UPROPERTY(EditAnywhere)
	FItemStates ItemStates;

	UPROPERTY(EditAnywhere)
	FItemFlags ItemFlags;

	UPROPERTY(EditAnywhere)
	FItemNumericData ItemNumericData;

	UPROPERTY(EditAnywhere)
	FItemAssetData ItemAssetData;

	UPROPERTY(EditAnywhere)
	FConsumAbleItemAssetData ConsumableItemAssetData;
	
	// 장착 가능한 무기 클래스 (스폰용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TSubclassOf<class ANS_BaseWeapon> WeaponActorClass;

	UPROPERTY(EditAnywhere)
	float SpawnWeight;

	//상한 음식 체크
	UPROPERTY(EditAnywhere)
	bool isSpoiledfood;

	/** 현재 수량 - 드롭/픽업 시 동기화용 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Quantity = 1;
	/** 무기 현재 탄약 수 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 CurrentAmmo = 0;
};