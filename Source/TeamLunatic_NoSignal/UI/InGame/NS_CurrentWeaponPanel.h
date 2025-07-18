// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/NS_WeaponType.h"
#include "NS_CurrentWeaponPanel.generated.h"

class UImage;
class UTextBlock;
class UNS_InventoryBaseItem;
class ANS_BaseWeapon;
class UNS_EquipedWeaponComponent;
class ANS_PlayerCharacterBase;

/**
 * 현재 장착된 무기 정보를 표시하는 UI 위젯
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_CurrentWeaponPanel : public UUserWidget
{
	GENERATED_BODY()
    
public:
	// 위젯 초기화
	virtual void NativeConstruct() override;
    
	// 매 프레임 업데이트
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
    
	// 현재 무기 정보 업데이트
	UFUNCTION(BlueprintCallable)
	void UpdateWeaponInfo(UNS_EquipedWeaponComponent* InEquipedWeaponComp);
    
	// 무기 타입에 따라 UI 표시 설정
	UFUNCTION(BlueprintCallable)
	void SetupUIByWeaponType(EWeaponType WeaponType, UNS_InventoryBaseItem* WeaponItem);
    
protected:

	UPROPERTY()
	ANS_PlayerCharacterBase* PlayerCharacter;

	UPROPERTY()
	UNS_EquipedWeaponComponent* EquipedWeaponComp;
	
	// 현재 장착된 무기의 ID를 저장하여 변경사항을 추적
	UPROPERTY()
	FName CurrentWeaponID;
	
	// 무기 아이콘 이미지
	UPROPERTY(meta = (BindWidget))
	UImage* WeaponIcon;
    
	// 현재 탄약 수 / 최대 탄약 수 텍스트 (원거리 무기용)
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentAndMaxAmmoText;
    
	// 발사 모드 텍스트 (원거리 무기용)
	UPROPERTY(meta = (BindWidget))
	UTextBlock* FireModeText;
    
	// 아이템 수량 텍스트 (던지는 무기용)
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemQuantityText;
};
