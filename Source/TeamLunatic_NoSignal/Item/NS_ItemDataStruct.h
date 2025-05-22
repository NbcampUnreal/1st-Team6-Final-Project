#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "NS_ItemDataStruct.generated.h"

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

UENUM()
enum class EWeaponType : uint8
{
	None,
	Melee,
	Ranged,
	Ammo
};

USTRUCT()
struct FItemTextData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	FText ItemName;

	UPROPERTY(EditAnywhere)
	FText ItemDescription;
	
	UPROPERTY(EditAnywhere)
	FText InteractionText;
};

USTRUCT()
struct FItemStates
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	int32 HealAmount;

	UPROPERTY(EditAnywhere)
	int32 StaminaRecovery;

	UPROPERTY(EditAnywhere)
	int32 HungerRestore;

	UPROPERTY(EditAnywhere)
	int32 ThirstRestore;

	UPROPERTY(EditAnywhere)
	int32 TiredRestore;
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
	USoundBase* UseSound = nullptr;
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
};


USTRUCT(BlueprintType)
struct FNS_ItemDataStruct : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	//있어야 될 데이터
	UPROPERTY(EditAnywhere, Category = "Item Data")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemTextData ItemTextData;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FWeaponData WeaponData;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemStates ItemStates;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemFlags ItemFlags;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemNumericData ItemNumericData;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemAssetData ItemAssetData;

	//상한 음식 체크
	UPROPERTY(EditAnywhere, Category = "Item Data")
	bool isSpoiledfood;
};