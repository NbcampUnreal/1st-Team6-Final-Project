// NS_DebugStatusWidget.cpp
#include "NS_DebugStatusWidget.h"
#include "Components/ProgressBar.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Character/Components/NS_StatusComponent.h"
#include "Kismet/GameplayStatics.h"

void UNS_DebugStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 플레이어 캐릭터와 스탯 컴포넌트 가져오기
	PlayerCharacter = Cast<ANS_PlayerCharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (PlayerCharacter)
	{
		StatusComp = PlayerCharacter->StatusComp;
	}
}

// 틱으로 캐릭터 스탯변화 업데이트
void UNS_DebugStatusWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!StatusComp) return;

	// 체력
	if (HealthBar)
	{
		HealthBar->SetPercent(StatusComp->Health / StatusComp->MaxHealth);
	}
	// 스태미너
	if (StaminaBar)
	{
		StaminaBar->SetPercent(StatusComp->Stamina / StatusComp->MaxStamina);
	}
	// 배고픔
	if (HungerBar)
	{
		HungerBar->SetPercent(StatusComp->CurrentHunger / StatusComp->MaxHunger);
	}
	// 갈증
	if (ThirstBar)
	{
		ThirstBar->SetPercent(StatusComp->CurrentThirst / StatusComp->MaxThirst);
	}
	// 졸림(피로)
	if (FatigueBar)
	{
		FatigueBar->SetPercent(StatusComp->CurrentFatigue / StatusComp->MaxFatigue);
	}
}
