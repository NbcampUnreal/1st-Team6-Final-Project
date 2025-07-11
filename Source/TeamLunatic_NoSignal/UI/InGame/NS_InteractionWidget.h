// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NS_InteractionWidget.generated.h"

// 전방 선언: 클래스 및 구조체 정의 전에 미리 선언하여 컴파일러에게 존재를 알립니다.
class ANS_PlayerCharacterBase; // 플레이어 캐릭터 클래스
struct FInteractableData;     // 상호작용 가능한 데이터 구조체
class UTextBlock;             // UMG 텍스트 블록 위젯
class UProgressBar;           // UMG 프로그레스 바 위젯

/**
 * @brief 상호작용 가능한 객체에 대한 정보를 표시하는 위젯입니다.
 *        객체의 이름, 수행할 액션, 수량, 상호작용 진행 바 등을 보여줍니다.
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_InteractionWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/**
	 * @brief 현재 플레이어 캐릭터에 대한 참조입니다.
	 *        상호작용 관련 정보를 가져오거나 업데이트하는 데 사용됩니다.
	 */
	UPROPERTY(VisibleAnywhere, Category = "Interaction Widget | Player Reference")
	ANS_PlayerCharacterBase* PlayerCharacter;

	/**
	 * @brief 위젯에 표시될 상호작용 데이터를 업데이트합니다.
	 * @param InteractableData 업데이트할 상호작용 데이터 구조체 포인터입니다.
	 *        이 함수는 위젯의 텍스트와 프로그레스 바를 갱신합니다.
	 */
	void UpdateWidget(const FInteractableData* InteractableData) const;

protected:
	/**
	 * @brief 상호작용 가능한 객체의 이름을 표시하는 텍스트 블록 위젯입니다.
	 *        블루프린트에서 바인딩됩니다.
	 */
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction Widget | Interactable Data")
	UTextBlock* NameText;

	/**
	 * @brief 상호작용 시 수행할 액션(예: '줍기', '열기')을 표시하는 텍스트 블록 위젯입니다.
	 *        블루프린트에서 바인딩됩니다.
	 */
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction Widget | Interactable Data")
	UTextBlock* ActionText;

	/**
	 * @brief 상호작용 가능한 아이템의 수량을 표시하는 텍스트 블록 위젯입니다.
	 *        블루프린트에서 바인딩됩니다.
	 */
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction Widget | Interactable Data")
	UTextBlock* QuantityText;

	/**
	 * @brief 상호작용을 위한 키 입력(예: 'E')을 표시하는 텍스트 블록 위젯입니다.
	 *        블루프린트에서 바인딩됩니다.
	 */
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction Widget | Interactable Data")
	UTextBlock* KeyPressText;

	/**
	 * @brief 상호작용 진행 상황을 시각적으로 보여주는 프로그레스 바 위젯입니다.
	 *        블루프린트에서 바인딩됩니다.
	 */
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction Widget | Interactable Data")
	UProgressBar* InteractionProgressBar;

	/**
	 * @brief 현재 상호작용이 진행된 시간 또는 진행률을 나타내는 변수입니다.
	 *        프로그레스 바 업데이트에 사용됩니다.
	 */
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction Widget | Interactable Data")
	float CurrentInteractionDuration;

	/**
	 * @brief 상호작용 진행 바의 현재 값을 계산하여 반환하는 함수입니다.
	 *        프로그레스 바의 퍼센티지를 업데이트하는 데 사용됩니다.
	 * @return 0.0에서 1.0 사이의 상호작용 진행률입니다.
	 */
	UFUNCTION(Category = "Interaction Widget | Interactable Data") 
	float UpdateInteractionProgress();

	/**
	 * @brief 위젯이 초기화될 때 호출되는 함수입니다.
	 *        주로 위젯의 초기 설정 및 변수 초기화에 사용됩니다.
	 */
	virtual void NativeOnInitialized() override;

	/**
	 * @brief 위젯이 생성될 때 호출되는 함수입니다.
	 *        주로 위젯의 초기 상태 설정 및 바인딩에 사용됩니다.
	 */
	virtual void NativeConstruct() override;
};
